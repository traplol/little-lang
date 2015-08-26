#include "table.h"

#include <stdlib.h>


unsigned int TableGetIdx(struct Table *table, void *thing) {
    return (unsigned long)thing % table->TableLength;
}

int TableMake(struct Table *table, const enum TableType tableType, const unsigned int tableMaxLen) {
    if (!table) {
        return -1;
    }

    if (0 == tableMaxLen) {
        table->TableLength = DEFAULT_TABLE_SIZE;
    }
    else {
        table->TableLength = tableMaxLen;
    }

    table->TableType = tableType;
    table->Entries = calloc(sizeof(*table->Entries), tableMaxLen);
    if (!table->Entries) {
        return -1;
    }
    return 0;
}
int TableFree(struct Table *table) {
    unsigned int i;
    if (!table) {
        return -1;
    }
    switch (table->TableType) {
        case TableTypeTypeTable:
            for (i = 0; i < table->TableLength; ++i) {
                if (table->Entries[i]) {
                    TypeInfoFree(table->Entries[i]->u.TypeInfo);
                }
            }
            break;
        case TableTypeValueTable:
            for (i = 0; i < table->TableLength; ++i) {
                if (table->Entries[i]) {
                    ValueFree(table->Entries[i]->u.Value);
                }
            }
            break;
    }
    return 0;
}
int TableEntryMake(struct TableEntry *tableEntry, struct Value *value, char *string, char *filename, int lineNumber, int columnNumber);


int TableInsert(struct Table *table, void *thing, char *string, char *filename, int lineNumber, int columnNumber) {
    struct TableEntry *entry;
    unsigned int tableIdx;
    if (!table || !table->Entries || !thing || !string || !filename) {
        return -1;
    }
    if (TableContains(table, thing)) {
        return -1;
    }

    entry = calloc(sizeof *entry, 1);
    if (!entry) {
        return -1;
    }

    switch (table->TableType) {
        case TableTypeTypeTable:
            entry->u.TypeInfo = thing;
            break;
        case TableTypeValueTable:
            entry->u.Value = thing;
            break;
    }
    entry->String = string;
    entry->Filename = filename;
    entry->LineNumber = lineNumber;
    entry->ColumnNumber = columnNumber;

    tableIdx = TableGetIdx(table, thing);
    table->Entries[tableIdx] = entry;
    return 0;
}
int TableRemove(struct Table *table, void *thing) {
    unsigned int tableIdx;
    if (!table || !table->Entries || !thing) {
        return -1;
    }
    if (!TableContains(table, thing)) {
        return 0;
    }
    tableIdx = TableGetIdx(table, thing);
    table->Entries[tableIdx] = NULL;
    return 0;
}

int TableContains(struct Table *table, void *thing) {
    if (!table || !table->Entries || !thing) {
        return 0;
    }
    unsigned int tableIdx;
    if (!table || !table->Entries || !thing) {
        return 0;
    }
    tableIdx = TableGetIdx(table, thing);
    return (int)(table->Entries[tableIdx]);
}
