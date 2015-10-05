#ifndef _LITTLE_LANG_SYMBOL_TABLE_H
#define _LITTLE_LANG_SYMBOL_TABLE_H

#include "src_loc.h"

struct Symbol {
    char *Key;
    int IsMutable;
    struct Value *Value;
    struct SrcLoc SrcLoc;
    struct Symbol *Next;
};

struct SymbolTable {
    struct Symbol **Symbols;
    unsigned int TableLength;
    struct SymbolTable *Parent;
    struct SymbolTable *Child;
};

int SymbolTableMakeGlobalScope(struct SymbolTable *table);
int SymbolTableFree(struct SymbolTable *table);

int SymbolTablePushScope(struct SymbolTable **table);
int SymbolTablePopScope(struct SymbolTable **table);

int SymbolTableAssign(struct SymbolTable *table, struct Value *value, char *key, int IsMutable, struct SrcLoc srcLoc);
int SymbolTableInsert(struct SymbolTable *table, struct Value *value, char *key, int IsMutable, struct SrcLoc srcLoc);
int SymbolTableFindLocal(struct SymbolTable *table, char *key, struct Symbol **out_symbol);
int SymbolTableFindNearest(struct SymbolTable *table, char *key, struct Symbol **out_symbol);

#endif
