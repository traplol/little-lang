#ifndef _LITTLE_LANG_RUNTIME_LITERAL_TABLE_H
#define _LITTLE_LANG_RUNTIME_LITERAL_TABLE_H

#include "value.h"

int LiteralLookupTableInsert(struct Value *literal);
int LiteralLookupTableGetIndex(struct Value *literal, int *out_index);
int LiteralTableGetValue(int index, struct Value **out_value);

#endif
