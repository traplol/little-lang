#ifndef _LITTLE_LANG_TABLE_H
#define _LITTLE_LANG_TABLE_H

#include "value.h"

#define DEFAULT_TABLE_SIZE 769

struct TableEntry {
    struct Value *Value;
    char *String;
    char *Filename;
    int LineNumber;
    int ColumnNumber;
    struct TableEntry *Next;
};

struct Table {
    struct TableEntry **Values;
    unsigned int TableLength;
};

/* Initializes the table. */
int TableMake(struct Table *table, const unsigned int tableMaxLen);
/* Frees all of the table's data. */
int TableFree(struct Table *table);
/* Inserts a value into the table. */
int TableInsert(struct Table *table, struct Value *value, char *string, char *filename, int lineNumber, int columnNumber);
/* Removes a value from the table. */
int TableRemove(struct Table *table, struct Value *value);
/* Returns a non-zero integer if the table contains the value. */
int TableContains(struct Table *table, struct Value *value);


#endif
