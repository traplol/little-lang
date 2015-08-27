#include "type_table.h"
#include "defines.h"

#include <stdlib.h>
#include <string.h>

/********************* Helpers **********************/

int TypeTableIsValid(struct TypeTable *table) {
    return table && table->Entries;
}

int TypeTableIsInvalid(struct TypeTable *table) {
    return !TypeTableIsValid(table);
}

struct TypeTableEntry *TypeTableEntryAlloc(char *key, struct TypeInfo *typeInfo) {
    struct TypeTableEntry *entry = malloc(sizeof *entry);
    entry->Key = key;
    entry->TypeInfo = typeInfo;
    entry->Next = NULL;
    return entry;
}

void TypeTableEntryFree(struct TypeTableEntry *entry) {
    struct TypeTableEntry *next;
    free(entry->Key);
    TypeInfoFree(entry->TypeInfo);
    next = entry->Next;
    while (next) {
        TypeTableEntryFree(next);
        next = next->Next;
    }
    free(entry);
}

unsigned int TypeTableGetIdx(struct TypeTable *table, char *key) {
    return (unsigned int)(key) % table->TableLength;
}

/********************* Public functions **********************/

int TypeTableMake(struct TypeTable *table, const unsigned int tableMaxLen) {
    if (!table) {
        return -1;
    }
    if (0 == tableMaxLen) {
        table->TableLength = DEFAULT_TABLE_SIZE;
    }
    else {
        table->TableLength = tableMaxLen;
    }
    table->Entries = calloc(sizeof(*table->Entries), table->TableLength);
    if (!table->Entries) {
        return -1;
    }
    return 0;
}
int TypeTableFree(struct TypeTable *table) {
    struct TypeTableEntry *entry;
    unsigned int i;
    if (TypeTableIsInvalid(table)) {
        return -1;
    }
    for (i = 0; i < table->TableLength; ++i) {
        entry = table->Entries[i];
        if (entry) {
            TypeTableEntryFree(entry);
        }
    }
    free(table->Entries);
    return 0;
}
int TypeTableInsert(struct TypeTable *table, struct TypeInfo *typeInfo) {
    struct TypeTableEntry *entry, *tmp;
    unsigned int tableIdx;
    if (TypeTableIsInvalid(table) || !typeInfo) {
        return -1;
    }

    entry = TypeTableEntryAlloc(typeInfo->TypeName, typeInfo);
    tableIdx = TypeTableGetIdx(table, typeInfo->TypeName);
    tmp = table->Entries[tableIdx];
    if (!tmp) {
        table->Entries[tableIdx] = entry;
        return 0;
    }
    while (tmp) {
        if (0 == strcmp(tmp->Key, typeInfo->TypeName)) {
            return -1;
        }
        tmp = tmp->Next;
    }
    tmp->Next = entry;
    return 0;
}
int TypeTableFind(struct TypeTable *table, char *key, struct TypeInfo **out_typeInfo) {
    struct TypeTableEntry *entry;
    unsigned int tableIdx;
    if (TypeTableIsInvalid(table)) {
        return 0;
    }

    tableIdx = TypeTableGetIdx(table, key);
    entry = table->Entries[tableIdx];
    if (!entry) {
        return 0;
    }

    while (entry) {
        if (0 == strcmp(entry->Key, key)) {
            break;
        }
        entry = entry->Next;
    }

    if (out_typeInfo) {
        *out_typeInfo = entry->TypeInfo;
    }
    return 1;
}
