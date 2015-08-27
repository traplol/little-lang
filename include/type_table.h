#ifndef _LITTLE_LANG_TYPE_TABLE_H
#define _LITTLE_LANG_TYPE_TABLE_H

#include "type_info.h"

struct TypeTableEntry {
    char *Key;
    struct TypeInfo *TypeInfo;
    struct TypeTableEntry *Next;
};

struct TypeTable {
    struct TypeTableEntry **Entries;
    unsigned int TableLength;
};

int TypeTableMake(struct TypeTable *table, const unsigned int tableMaxLen);
int TypeTableFree(struct TypeTable *table);
int TypeTableInsert(struct TypeTable *table, struct TypeInfo *typeInfo);
int TypeTableFind(struct TypeTable *table, char *key, struct TypeInfo **out_typeInfo);

#endif
