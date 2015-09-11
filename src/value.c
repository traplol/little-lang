#include "value.h"
#include "globals.h"
#include "result.h"

#include "helpers/strings.h"

#include <string.h>
#include <stdlib.h>

struct Value *ValueAllocBlank(void) {
    struct Value *v = calloc(sizeof *v, 1);
    return v;
}
struct Value *ValueAlloc(void) {
    struct Value *v = malloc(sizeof *v);
    return v;
}

/********************* Public Functions *********************/

int ValueDuplicate(struct Value **out_value, struct Value *toDup) {
    if (!out_value || !toDup) {
        return R_InvalidArgument;
    }
    struct Value *out = ValueAlloc();
    memcpy(out, toDup, sizeof *out);
    *out_value = out;
    return R_OK;
}

int BuiltinFnMake(struct BuiltinFn **out_builtin_fn, char *name, int numArgs, int isVarArgs, BuiltinFnProc_t fn) {
    struct BuiltinFn *bifn;
    if (!out_builtin_fn || !name || !fn) {
        return R_InvalidArgument;
    }
    bifn = malloc(sizeof *bifn);
    bifn->Name = name;
    bifn->NumArgs = numArgs;
    bifn->IsVarArgs = isVarArgs;
    bifn->Fn = fn;
    *out_builtin_fn = bifn;
    return R_OK;
}
int FunctionMake(struct Function **out_function, char *name, int numArgs, int isVarArgs, struct Ast *params, struct Ast *body) {
    struct Function *function;
    if (!out_function || !name) {
        return R_InvalidArgument;
    }
    function = malloc(sizeof *function);
    function->Name = name;
    function->NumArgs = numArgs;
    function->IsVarArgs = isVarArgs;
    function->Params = params;
    function->Body = body;
    *out_function = function;
    return R_OK;
}

int ValueMakeInteger(struct Value *value, int integer) {
    if (!value) {
        return R_InvalidArgument;
    }
    value->TypeInfo = &g_TheIntegerTypeInfo;
    value->IsBuiltInFn = 0;
    value->IsPassByReference = 0;
    value->v.Integer = integer;
    return R_OK;
}
int ValueMakeReal(struct Value *value, double real) {
    if (!value) {
        return R_InvalidArgument;
    }
    value->TypeInfo = &g_TheRealTypeInfo;
    value->IsBuiltInFn = 0;
    value->IsPassByReference = 0;
    value->v.Real = real;
    return R_OK;
}
int ValueMakeObject(struct Value *value, struct TypeInfo *typeInfo, void *object, unsigned int objectSize) {
    if (!value || !typeInfo) {
        return R_InvalidArgument;
    }
    value->TypeInfo = typeInfo;
    value->IsBuiltInFn = 0;
    value->IsPassByReference = 1;
    if (object && objectSize) {
        memcpy(value->v.__ptrsize, object, objectSize);
    }
    return R_OK;
}
int ValueMakeLLString(struct Value *value, char *cString) {
    if (!value || !cString) {
        return R_InvalidArgument;
    }
    value->TypeInfo = &g_TheStringTypeInfo;
    value->IsBuiltInFn = 0;
    value->IsPassByReference = 1;
    value->v.String = malloc(sizeof *(value->v.String));
    value->v.String->CString = strdup(cString);
    value->v.String->Length = strlen(value->v.String->CString);
    return R_OK;
}
int ValueMakeFunction(struct Value *value, struct Function *function) {
    if (!value || !function) {
        return R_InvalidArgument;
    }
    value->TypeInfo = &g_TheFunctionTypeInfo;
    value->IsBuiltInFn = 0;
    value->IsPassByReference = 0;
    value->v.Function = function;
    return R_OK;
}

int ValueMakeBuiltinFn(struct Value *value, struct BuiltinFn *builtinFn) {
    if (!value || !builtinFn) {
        return R_InvalidArgument;
    }
    value->TypeInfo = &g_TheBuiltinFnTypeInfo;
    value->IsBuiltInFn = 1;
    value->IsPassByReference = 0;
    value->v.BuiltinFn = builtinFn;
    return R_OK;
}
