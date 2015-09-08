#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

#include "type_info.h"
#include "ast.h"

struct Function {
    char *Name;
    struct Ast *Params;
    struct Ast *Body;
};

typedef struct Value *(*BuiltinFn_t)(unsigned int argc, struct Value **argv);

struct Value {
    struct TypeInfo *TypeInfo;
    int IsBuiltInFn;
    union {
        int Integer;
        double Real;
        void *UserObject;
        struct Function *Function;
        BuiltinFn_t BuiltinFn;
        unsigned char __ptrsize[sizeof(void*)];
    } v;
};

int ValueMake(struct Value *value, struct TypeInfo *typeInfo, void *data, unsigned int dataSize);
int ValueMakeBuiltinFn(struct Value *value, struct TypeInfo *typeInfo, BuiltinFn_t builtinFn);

#endif
