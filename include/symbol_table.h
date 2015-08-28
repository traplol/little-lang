#ifndef _LITTLE_LANG_SYMBOL_TABLE_H
#define _LITTLE_LANG_SYMBOL_TABLE_H

struct Symbol {
    char *Key;
    struct Value *Value;
    char *Filename;
    int LineNumber;
    int ColumnNumber;
    struct Symbol *Next;
};

struct SymbolTable {
    struct Symbol **Symbols;
    unsigned int TableLength;
    struct SymbolTable *Parent;
};

int SymbolTableMakeGlobalScope(struct SymbolTable *table);
int SymbolTableFree(struct SymbolTable *table);

int SymbolTablePushScope(struct SymbolTable **table);
int SymbolTablePopScope(struct SymbolTable **table);

int SymbolTableInsert(struct SymbolTable *table, struct Value *value, char *key, char *filename, int lineNumber, int columnNumber);
int SymbolTableFindLocal(struct SymbolTable *table, char *key, struct Symbol **out_symbol);
int SymbolTableFindNearest(struct SymbolTable *table, char *key, struct Symbol **out_symbol);

#endif
