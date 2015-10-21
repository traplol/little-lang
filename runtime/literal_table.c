#include "runtime/literal_table.h"
#include "result.h"

#include <stdlib.h>

static int LiteralTableCurrentIndex = 0;
static int LiteralTableCapacity;
static struct Value **LiteralTable;

int LiteralTableGetValue(int index, struct Value **out_value) {
    if (!out_value) {
        *out_value = NULL;
        return R_InvalidArgument;
    }
    if (index >= LiteralTableCurrentIndex) {
        *out_value = NULL;
        return R_InvalidArgument;
    }
    if (index < 0) {
        *out_value = NULL;
        return R_InvalidArgument;
    }
    *out_value = LiteralTable[index];
    return R_OK;
}

static void LiteralTableResize(void) {
    int i;
    struct Value **newTable;
    LiteralTableCapacity |= 1; /* This covers the case of unitialized table */
    LiteralTableCapacity <<= 1; /* Double size */
    newTable = calloc(sizeof *newTable, LiteralTableCapacity);
    for (i = 0; i < LiteralTableCurrentIndex; ++i) {
        newTable[i] = LiteralTable[i];
    }
    free(LiteralTable);
    LiteralTable = newTable;
}

static int LiteralTableAppend(struct Value *literal) {
    if (LiteralTableCurrentIndex + 1 >= LiteralTableCapacity) {
        LiteralTableResize();
    }
    LiteralTable[LiteralTableCurrentIndex] = literal;
    return LiteralTableCurrentIndex++;
}


struct LiteralLookupTableNode {
    struct Value *Key;
    int Index;
    struct LiteralLookupTableNode *Next;
};

#define LITERAL_LOOKUP_TABLE_LENGTH 769
static struct LiteralLookupTableNode *LiteralLookupTable[LITERAL_LOOKUP_TABLE_LENGTH];

static inline unsigned int LiteralLookupTableIndex(struct Value *literal) {
    return (unsigned int)literal & LITERAL_LOOKUP_TABLE_LENGTH;
}

int LiteralLookupTableGetIndex(struct Value *literal, int *out_index) {
    struct LiteralLookupTableNode *node;
    unsigned int idx;
    if (!literal || !out_index) {
        return R_InvalidArgument;
    }
    idx = LiteralLookupTableIndex(literal);
    node = LiteralLookupTable[idx];
    while (node) {
        if (literal == node->Key) {
            *out_index = node->Index;
            return R_OK;
        }
        node = node->Next;
    }
    *out_index = -1;
    return R_KeyNotInTable;
}

int LiteralLookupTableInsert(struct Value *literal) {
    struct LiteralLookupTableNode *node;
    unsigned int idx;
    if (!literal) {
        return R_InvalidArgument;
    }
    idx = LiteralLookupTableIndex(literal);
    node = LiteralLookupTable[idx];
    if (!node) {
        node = calloc(sizeof *node, 1);
        node->Index = LiteralTableAppend(literal);
        node->Key = literal;
        LiteralLookupTable[idx] = node;
        return R_OK;
    }
    else if (node->Key == literal) {
        return R_OK;
    }

    while (node->Next) {
        if (literal == node->Key) {
            return R_OK;
        }
        node = node->Next;
    }

    node->Next = calloc(sizeof *node, 1);
    node = node->Next;
    node->Index = LiteralTableCurrentIndex++;
    node->Key = literal;
    return R_OK;
}
