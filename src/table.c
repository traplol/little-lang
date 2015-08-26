#include "table.h"

#include <stdlib.h>


unsigned int TableGetIdx(struct Table *table, struct Value *value) {
    return (unsigned long)value % table->TableLength;
}

int TableMake(struct Table *table, const unsigned int tableMaxLen) {
    if (!table) {
        return -1;
    }

    if (0 == tableMaxLen) {
        table->TableLength = DEFAULT_TABLE_SIZE;
    }
    else {
        table->TableLength = tableMaxLen;
    }

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
    for (i = 0; i < table->TableLength; ++i) {
        if (table->Entries[i]) {
            free(table->Entries[i]);
        }
    }
    return 0;
}
int TableEntryMake(struct TableEntry *tableEntry, struct Value *value, char *string, char *filename, int lineNumber, int columnNumber);

int TableInsert(struct Table *table, struct Value *value, char *string, char *filename, int lineNumber, int columnNumber) {
    struct TableEntry *entry;
    unsigned int tableIdx;
    if (!table || !table->Entries || !value || !string || !filename) {
        return -1;
    }

    if (TableContains(table, value)) {
        return -1;
    }

    entry = calloc(sizeof *entry, 1);
    if (!entry) {
        return -1;
    }

    entry->Value = value;
    entry->String = string;
    entry->Filename = filename;
    entry->LineNumber = lineNumber;
    entry->ColumnNumber = columnNumber;

    tableIdx = TableGetIdx(table, value);
    table->Entries[tableIdx] = entry;
    return 0;
}
int TableRemove(struct Table *table, struct Value *value) {
    unsigned int tableIdx;
    if (!table || !table->Entries || !value) {
        return -1;
    }
    if (!TableContains(table, value)) {
        return 0;
    }
    tableIdx = TableGetIdx(table, value);
    table->Entries[tableIdx] = NULL;
    return 0;
}
int TableContains(struct Table *table, struct Value *value) {
    unsigned int tableIdx;
    if (!table || !table->Entries || !value) {
        return 0;
    }
    tableIdx = TableGetIdx(table, value);
    return (int)(table->Entries[tableIdx]);
}
