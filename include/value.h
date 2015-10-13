#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

#include "type_info.h"
#include "symbol_table.h"
#include "ast.h"

#include <stdint.h>

struct Function {
    unsigned int NumArgs;
    int IsVarArgs;
    char *Name;
    struct Ast *Params;
    struct Ast *Body;
    struct Module *OwnerModule;
};

typedef struct Value *(*BuiltinFnProc_t)(struct Module *module, unsigned int argc, struct Value **argv);
struct BuiltinFn {
    char *Name;
    unsigned int NumArgs;
    int IsVarArgs;
    BuiltinFnProc_t Fn;
};

struct LLString {
    unsigned int Length;
    char *CString;
};

struct LLVector {
    unsigned int Length;
    unsigned int Capacity;
    struct Value **Values;
};

struct Value {
    struct TypeInfo *TypeInfo;
    int IsBuiltInFn : 1,
        IsPassByReference : 1,
        IsSymbol : 1,
        Visited : 1;
    union {
        int Integer;
        uint64_t RealToIntBits;
        double Real;
        struct TypeInfo *MetaTypeInfo;
        struct Symbol *Symbol;
        struct LLString *String;
        struct Function *Function;
        struct BuiltinFn *BuiltinFn;
        unsigned char __ptrsize[sizeof(void*)];
    } v;
    struct SymbolTable *Members;
};

struct Value *ValueAlloc(void);
struct Value *ValueAllocNoGC(void);
int ValueFree(struct Value *value);

int BuiltinFnMake(struct BuiltinFn **out_builtin_fn, char *name, unsigned int numArgs, int isVarArgs, BuiltinFnProc_t proc);
int FunctionMake(struct Function **out_function, char *name, unsigned int numArgs, int isVarArgs, struct Ast *params, struct Ast *body);

int ValueDuplicate(struct Value **out_value, struct Value *toDup);

int ValueMakeIntegerLiteral(struct Value **out_value, int integer);
int ValueMakeRealLiteral(struct Value **out_value, double real);
int ValueMakeLLStringLiteral(struct Value **out_value, char *cString);

int ValueMakeInteger(struct Value **out_value, int integer);
int ValueMakeReal(struct Value **out_value, double real);
int ValueMakeObject(struct Value **out_value, struct TypeInfo *typeInfo);
int ValueMakeType(struct Value **out_value, struct TypeInfo *typeInfo);
int ValueMakeLLString(struct Value **out_value, char *cString);
int ValueMakeSingleton(struct Value *value, struct TypeInfo *typeInfo);
int ValueMakeFunction(struct Value *value, struct Function *function);
int ValueMakeBuiltinFn(struct Value *value, struct BuiltinFn *builtinFn);

char *ValueToString(struct Value *value);

#endif
