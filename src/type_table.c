#include "type_table.h"
#include "helpers/strings.h"
#include "result.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_TABLE_SIZE 769
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
    TypeInfoFree(entry->TypeInfo);
    next = entry->Next;
    while (next) {
        TypeTableEntryFree(next);
        next = next->Next;
    }
    free(entry);
}

unsigned int TypeTableGetIdx(struct TypeTable *table, char *key) {
    return string_hash(key) % table->TableLength;
}

/********************* Public functions **********************/

int TypeTableMake(struct TypeTable *table, const unsigned int tableMaxLen) {
    if (!table) {
        return R_InvalidArgument;
    }
    if (0 == tableMaxLen) {
        table->TableLength = DEFAULT_TABLE_SIZE;
    }
    else {
        table->TableLength = tableMaxLen;
    }
    table->Entries = calloc(sizeof(*table->Entries), table->TableLength);
    if (!table->Entries) {
        return R_AllocFailed;
    }
    return R_OK;
}
int TypeTableFree(struct TypeTable *table) {
    struct TypeTableEntry *entry;
    unsigned int i;
    if (TypeTableIsInvalid(table)) {
        return R_InvalidArgument;
    }
    for (i = 0; i < table->TableLength; ++i) {
        entry = table->Entries[i];
        if (entry) {
            TypeTableEntryFree(entry);
        }
    }
    free(table->Entries);
    return R_OK;
}
int TypeTableInsert(struct TypeTable *table, struct TypeInfo *typeInfo) {
    struct TypeTableEntry *entry, *tmp;
    struct TypeInfo *ti;
    unsigned int tableIdx;
    if (TypeTableIsInvalid(table) || !typeInfo) {
        return R_InvalidArgument;
    }
    TypeTableFind(table, typeInfo->TypeName, &ti);
    if (ti) {
        return R_KeyAlreadyInTable;
    }

    entry = TypeTableEntryAlloc(typeInfo->TypeName, typeInfo);
    tableIdx = TypeTableGetIdx(table, typeInfo->TypeName);
    tmp = table->Entries[tableIdx];
    if (!tmp) {
        table->Entries[tableIdx] = entry;
        return R_OK;
    }
    while (tmp->Next) {
        if (0 == strcmp(tmp->Key, typeInfo->TypeName)) {
            return R_KeyAlreadyInTable;
        }
        tmp = tmp->Next;
    }
    tmp->Next = entry;
    return R_OK;
}
int TypeTableFind(struct TypeTable *table, char *key, struct TypeInfo **out_typeInfo) {
    struct TypeTableEntry *entry;
    unsigned int tableIdx;
    if (TypeTableIsInvalid(table)) {
        *out_typeInfo = NULL;
        return R_False;
    }

    tableIdx = TypeTableGetIdx(table, key);
    entry = table->Entries[tableIdx];
    if (!entry) {
        *out_typeInfo = NULL;
        return R_False;
    }

    while (entry) {
        if (0 == strcmp(entry->Key, key)) {
            *out_typeInfo = entry->TypeInfo;
            return R_True;
        }
        entry = entry->Next;
    }
    *out_typeInfo = NULL;
    return R_False;
}
