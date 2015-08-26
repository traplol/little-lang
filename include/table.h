#ifndef _LITTLE_LANG_TABLE_H
#define _LITTLE_LANG_TABLE_H

#include "value.h"
#include "defines.h"

enum TableType {
    TableTypeTypeTable,
    TableTypeValueTable
};

struct TableEntry {
    enum TableType TableEntryType;
    union {
        struct TypeInfo *TypeInfo;
        struct Value *Value;
    } u;
    char *String;
    char *Filename;
    int LineNumber;
    int ColumnNumber;
    struct TableEntry *Next;
};

struct Table {
    enum TableType TableType;
    struct TableEntry **Entries;
    unsigned int TableLength;
};

/* Initializes the table. */
int TableMake(struct Table *table, const enum TableType tableType, const unsigned int tableMaxLen);
/* Frees all of the table's data. */
int TableFree(struct Table *table);
/* Inserts a value into the table. */
int TableInsert(struct Table *table, void *thing, char *string, char *filename, int lineNumber, int columnNumber);
/* Removes a value from the table. */
int TableRemove(struct Table *table, void *value);
/* Returns a non-zero integer if the table contains the value. */
int TableContains(struct Table *table, void *value);


#endif
