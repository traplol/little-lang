#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

#include "type_info.h"
#include "ast.h"

struct Function {
    int NumArgs;
    int IsVarArgs;
    char *Name;
    struct Ast *Params;
    struct Ast *Body;
    struct Module *OwnerModule;
};

typedef struct Value *(*BuiltinFnProc_t)(unsigned int argc, struct Value **argv);
struct BuiltinFn {
    char *Name;
    int NumArgs;
    int IsVarArgs;
    BuiltinFnProc_t Fn;
};

struct LLString {
    unsigned int Length;
    char *CString;
};


struct Value {
    struct TypeInfo *TypeInfo;
    int IsBuiltInFn;
    int IsPassByReference;
    union {
        int Integer;
        double Real;
        void *Object;
        struct LLString *String;
        struct Function *Function;
        struct BuiltinFn *BuiltinFn;
        unsigned char __ptrsize[sizeof(void*)];
    } v;
};

struct Value *ValueAllocBlank(void);
struct Value *ValueAlloc(void);

int BuiltinFnMake(struct BuiltinFn **out_builtin_fn, char *name, int numArgs, int isVarArgs, BuiltinFnProc_t proc);
int FunctionMake(struct Function **out_function, char *name, int numArgs, int isVarArgs, struct Ast *params, struct Ast *body);

int ValueDuplicate(struct Value **out_value, struct Value *toDup);

int ValueMakeInteger(struct Value *value, int integer);
int ValueMakeReal(struct Value *value, double real);
int ValueMakeObject(struct Value *value, struct TypeInfo *typeInfo, void *object, unsigned int objectSize);
int ValueMakeLLString(struct Value *value, char *cString);
int ValueMakeFunction(struct Value *value, struct Function *function);
int ValueMakeBuiltinFn(struct Value *value, struct BuiltinFn *builtinFn);

char *ValueToString(struct Value *value);

#endif
