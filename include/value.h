#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

#include "type_info.h"

struct Value {
    struct TypeInfo *TypeInfo;
    union {
        int Integer;
        double Real;
        void *UserObject;
        unsigned char __ptrsize[sizeof(void*)];
    } v;
};

int ValueMake(struct Value *value, struct TypeInfo *typeInfo, void *data, unsigned int dataSize);

#endif
