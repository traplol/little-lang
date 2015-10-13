#include "symbol_table.h"

#include "helpers/strings.h"

#include "result.h"

#include <stdlib.h>
#include <string.h>

#define GLOBAL_SCOPE_SYMBOL_TABLE_LENGTH 769U  /* ~0.13% collision rate */
#define DEFAULT_SYMBOL_TABLE_LENGTH 53U    /* ~10.4% collision rate */

/****************** Helpers *******************/
struct Symbol **SymbolTableAllocSymbols(unsigned int len) {
    return calloc(sizeof(struct Symbol*), len);
}
struct Symbol *SymbolAlloc(char *key, struct Value *value, int isMutable, struct SrcLoc srcLoc) {
    struct Symbol *symbol = malloc(sizeof *symbol);
    symbol->IsMutable = isMutable;
    symbol->Key = strdup(key);
    symbol->Value = value;
    symbol->SrcLoc = srcLoc;
    symbol->Next = NULL;
    return symbol;
}

void SymbolFree(struct Symbol *symbol) {
    free(symbol->Key);
}

int SymbolTableIsValid(struct SymbolTable *table) {
    return table && table->Symbols;
}

int SymbolTableIsInvalid(struct SymbolTable *table) {
    return !SymbolTableIsValid(table);
}

unsigned int SymbolTableGetIdx(struct SymbolTable *table, char *key) {
    return string_hash(key) % table->TableLength;
}

/****************** Public Functions *******************/

/* TODO: At some point we should make a structure called ScopeTable that holds
   on to the `Parent' and `Child' scopes, rather than having them a part of 
   the `SymbolTable' structure. */

int SymbolTableMakeGlobalScope(struct SymbolTable *table) {
    if (!table) {
        return R_InvalidArgument;
    }
    table->TableLength = GLOBAL_SCOPE_SYMBOL_TABLE_LENGTH;
    table->Symbols = SymbolTableAllocSymbols(table->TableLength);
    table->Parent = NULL;
    table->Child = NULL;
    return R_OK;
}

int SymbolTableMake(struct SymbolTable *table) {
    if (!table) {
        return R_InvalidArgument;
    }
    table->TableLength = DEFAULT_SYMBOL_TABLE_LENGTH;
    table->Symbols = SymbolTableAllocSymbols(table->TableLength);
    table->Parent = NULL;
    table->Child = NULL;
    return R_OK;
}

int SymbolTableFree(struct SymbolTable *table) {
    unsigned int i;
    struct Symbol *symbol;
    if (SymbolTableIsInvalid(table)) {
        return R_InvalidArgument;
    }
    for (i = 0; i < table->TableLength; ++i) {
        symbol = table->Symbols[i];
        if (symbol) {
            SymbolFree(symbol);
            free(symbol);
        }
    }
    table->Parent = NULL;
    table->Child = NULL;
    free(table->Symbols);
    return R_OK;
}

int SymbolTablePushScope(struct SymbolTable **table) {
    struct SymbolTable *newScope;
    if (SymbolTableIsInvalid(*table)) {
        return R_InvalidArgument;
    }

    newScope = malloc(sizeof *newScope);
    SymbolTableMake(newScope);
    newScope->Parent = *table;
    (*table)->Child = newScope;
    *table = newScope;
    return R_OK;
}

int SymbolTablePopScope(struct SymbolTable **table) {
    struct SymbolTable *oldScope;
    if (SymbolTableIsInvalid(*table)) {
        return R_InvalidArgument;
    }

    oldScope = *table;
    *table = (*table)->Parent;
    (*table)->Child = NULL;
    SymbolTableFree(oldScope);
    free(oldScope);
    return R_OK;
}

int SymbolTableAssign(struct SymbolTable *table, struct Value *value, char *key, int isMutable, struct SrcLoc srcLoc) {
    struct Symbol *symbol;
    int result = SymbolTableInsert(table, value, key, isMutable, srcLoc);
    if (R_OK == result) {
        return result;
    }
    result = SymbolTableFindLocal(table, key, &symbol);
    symbol->Value = value;
    return result;
}

int SymbolTableInsert(struct SymbolTable *table, struct Value *value, char *key, int isMutable, struct SrcLoc srcLoc) {
    struct Symbol *symbol, *tmp;
    unsigned int tableIdx;
    if (SymbolTableIsInvalid(table) || !value || !key || !srcLoc.Filename) {
        return R_InvalidArgument;
    }

    tableIdx = SymbolTableGetIdx(table, key);
    tmp = table->Symbols[tableIdx];
    if (!tmp) {
        symbol = SymbolAlloc(key, value, isMutable, srcLoc);
        table->Symbols[tableIdx] = symbol;
        return R_OK;
    }
    while (tmp->Next) {
        if (0 == strcmp(tmp->Key, key)) {
            return R_KeyAlreadyInTable;
        }
        tmp = tmp->Next;
    }
    symbol = SymbolAlloc(key, value, isMutable, srcLoc);
    tmp->Next = symbol;
    return R_OK;
}

int SymbolTableFindLocal(struct SymbolTable *table, char *key, struct Symbol **out_symbol) {
    struct Symbol *symbol;
    unsigned int tableIdx;
    if (SymbolTableIsInvalid(table)) {
        *out_symbol = NULL;
        return R_InvalidArgument;
    }

    tableIdx = SymbolTableGetIdx(table, key);
    symbol = table->Symbols[tableIdx];
    if (!symbol) {
        *out_symbol = NULL;
        return R_False;
    }

    while (symbol) {
        if (0 == strcmp(symbol->Key, key)) {
            break;
        }
        symbol = symbol->Next;
    }

    if (out_symbol) {
        *out_symbol = symbol;
    }
    return R_True;
}

int SymbolTableFindNearest(struct SymbolTable *table, char *key, struct Symbol **out_symbol) {
    struct Symbol *symbol = NULL;
    if (SymbolTableIsInvalid(table) || !key) {
        return R_InvalidArgument;
    }
    while (table) {
        if (SymbolTableFindLocal(table, key, &symbol)) {
            break;
        }
        table = table->Parent;
    }
    if (!symbol) {
        *out_symbol = NULL;
        return R_False;
    }
    if (out_symbol) {
        *out_symbol = symbol;
    }
    return R_True;
}
