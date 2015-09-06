#include "symbol_table.h"
#include "helpers/strings.h"
#include "result.h"

#include <stdlib.h>
#include <string.h>

#define GLOBAL_SCOPE_SYMBOL_TABLE_LENGTH 769U  /* ~0.13% collision rate */
#define LOCAL_SCOPE_SYMBOL_TABLE_LENGTH 53U    /* ~10.4% collision rate */

/****************** Helpers *******************/
struct Symbol **SymbolTableAllocSymbols(unsigned int len) {
    return calloc(sizeof(struct Symbol*), len);
}
struct Symbol *SymbolAlloc(char *key, struct Value *value, char *filename, int lineNumber, int columnNumber) {
    struct Symbol *symbol = malloc(sizeof *symbol);
    symbol->Key = key;
    symbol->Value = value;
    symbol->Filename = filename;
    symbol->LineNumber = lineNumber;
    symbol->ColumnNumber = columnNumber;
    symbol->Next = NULL;
    return symbol;
}

void SymbolFree(struct Symbol *symbol) {
    free(symbol->Key);
    free(symbol);
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

int SymbolTableMakeGlobalScope(struct SymbolTable *table) {
    if (!table) {
        return R_InvalidArgument;
    }
    table->TableLength = GLOBAL_SCOPE_SYMBOL_TABLE_LENGTH;
    table->Symbols = SymbolTableAllocSymbols(table->TableLength);
    table->Parent = NULL;
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
        }
    }
    table->Parent = NULL;
    free(table->Symbols);
    return R_OK;
}

int SymbolTablePushScope(struct SymbolTable **table) {
    struct SymbolTable *newScope;
    if (SymbolTableIsInvalid(*table)) {
        return R_InvalidArgument;
    }

    newScope = malloc(sizeof *newScope);
    newScope->TableLength = LOCAL_SCOPE_SYMBOL_TABLE_LENGTH;
    newScope->Symbols = SymbolTableAllocSymbols(newScope->TableLength);
    newScope->Parent = *table;
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
    SymbolTableFree(oldScope);
    free(oldScope);
    return R_OK;
}

int SymbolTableInsert(struct SymbolTable *table, struct Value *value, char *key, char *filename, int lineNumber, int columnNumber) {
    struct Symbol *symbol, *tmp;
    unsigned int tableIdx;
    if (SymbolTableIsInvalid(table) || !value || !key || !filename) {
        return R_InvalidArgument;
    }

    symbol = SymbolAlloc(key, value, filename, lineNumber, columnNumber);
    tableIdx = SymbolTableGetIdx(table, key);
    tmp = table->Symbols[tableIdx];
    if (!tmp) {
        table->Symbols[tableIdx] = symbol;
        return R_OK;
    }
    while (tmp) {
        if (0 == strcmp(tmp->Key, key)) {
            return R_KeyAlreadyInTable;
        }
        tmp = tmp->Next;
    }
    tmp->Next = symbol;
    return R_OK;
}

int SymbolTableFindLocal(struct SymbolTable *table, char *key, struct Symbol **out_symbol) {
    struct Symbol *symbol;
    unsigned int tableIdx;
    if (SymbolTableIsInvalid(table)) {
        return R_InvalidArgument;
    }

    tableIdx = SymbolTableGetIdx(table, key);
    symbol = table->Symbols[tableIdx];
    if (!symbol) {
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
        return R_False;
    }
    if (out_symbol) {
        *out_symbol = symbol;
    }
    return R_True;
}
