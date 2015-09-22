#include "value.h"
#include "globals.h"
#include "result.h"
#include "symbol_table.h"

#include "runtime/gc.h"

#include "helpers/strings.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void ValueDefaults(struct Value *value) {
    value->IsBuiltInFn = 0;
    value->IsPassByReference = 0;
    value->IsSymbol = 0;
}

int BuiltinFnFree(struct BuiltinFn *bifn) {
    if (!bifn) {
        return R_InvalidArgument;
    }
    free(bifn->Name);
    return R_OK;
}
int FunctionFree(struct Function *function) {
    if (!function) {
        return R_InvalidArgument;
    }
    free(function->Name);
    AstFree(function->Params);
    free(function->Params);
    AstFree(function->Body);
    free(function->Body);
    return R_OK;
}
int LLStringFree(struct LLString *llString) {
    if (!llString) {
        return R_InvalidArgument;
    }
    free(llString->CString);
    return R_OK;
}

/********************* Public Functions *********************/

struct Value *ValueAllocBlank(void) {
    struct Value *v;
    GC_AllocValue(&v);
    return v;
}
struct Value *ValueAlloc(void) {
    return ValueAllocBlank();
}
int ValueFree(struct Value *value) {
    int result;
    if (!value) {
        return R_InvalidArgument;
    }
    if (value->IsSymbol) {
        return R_OK;
    }
    else if (value->IsBuiltInFn) {
        result = BuiltinFnFree(value->v.BuiltinFn);
        free(value->v.BuiltinFn);
        return result;
    }
    else {
        switch (value->TypeInfo->Type) {
            case TypeBaseObject:
            case TypeBoolean:
            case TypeInteger:
            case TypeReal:
            case TypeUserObject:
                return R_OK;
            case TypeString:
                return LLStringFree(value->v.String);
            case TypeFunction:
                return FunctionFree(value->v.Function);
        }
    }
    return R_OperationFailed;
}

int ValueDuplicate(struct Value **out_value, struct Value *toDup) {
    struct Value *out;
    if (!out_value || !toDup) {
        return R_InvalidArgument;
    }
    if (toDup->IsPassByReference) {
        *out_value = toDup;
        toDup->Count++;
    }
    else {
        out = ValueAlloc();
        memcpy(out, toDup, sizeof *out);
        *out_value = out;
    }
    return R_OK;
}

int BuiltinFnMake(struct BuiltinFn **out_builtin_fn, char *name, unsigned int numArgs, int isVarArgs, BuiltinFnProc_t fn) {
    struct BuiltinFn *bifn;
    if (!out_builtin_fn || !name || !fn) {
        return R_InvalidArgument;
    }
    bifn = malloc(sizeof *bifn);
    bifn->Name = strdup(name);
    bifn->NumArgs = numArgs;
    bifn->IsVarArgs = isVarArgs;
    bifn->Fn = fn;
    *out_builtin_fn = bifn;
    return R_OK;
}
int FunctionMake(struct Function **out_function, char *name, unsigned int numArgs, int isVarArgs, struct Ast *params, struct Ast *body) {
    struct Function *function;
    if (!out_function || !name) {
        return R_InvalidArgument;
    }
    if (!params) {
        params = calloc(sizeof *params, 1);
    }
    function = malloc(sizeof *function);
    function->Name = strdup(name);
    function->NumArgs = numArgs;
    function->IsVarArgs = isVarArgs;
    function->Params = params;
    function->Body = body;
    *out_function = function;
    return R_OK;
}

int ValueMakeInteger(struct Value **out_value, int integer) {
    struct Value *value;
    if (!out_value) {
        return R_InvalidArgument;
    }
    value = ValueAlloc();
    value->TypeInfo = &g_TheIntegerTypeInfo;
    value->v.Integer = integer;
    *out_value = value;
    return R_OK;
}
int ValueMakeReal(struct Value **out_value, double real) {
    struct Value *value;
    if (!out_value) {
        return R_InvalidArgument;
    }
    value = ValueAlloc();
    value->TypeInfo = &g_TheRealTypeInfo;
    value->v.Real = real;
    *out_value = value;
    return R_OK;
}
int ValueMakeObject(struct Value *value, struct TypeInfo *typeInfo, void *object, unsigned int objectSize) {
    if (!value || !typeInfo) {
        return R_InvalidArgument;
    }
    ValueDefaults(value);
    value->TypeInfo = typeInfo;
    value->IsPassByReference = 1;
    if (object && objectSize) {
        memcpy(value->v.__ptrsize, object, objectSize);
    }
    return R_OK;
}
int ValueMakeLLString(struct Value **out_value, char *cString) {
    struct Value *value;
    if (!out_value || !cString) {
        return R_InvalidArgument;
    }
    value = ValueAlloc();
    value->TypeInfo = &g_TheStringTypeInfo;
    value->IsPassByReference = 1;
    value->v.String = malloc(sizeof *(value->v.String));
    value->v.String->CString = strdup(cString);
    value->v.String->Length = strlen(value->v.String->CString);
    *out_value = value;
    return R_OK;
}
int ValueMakeFunction(struct Value *value, struct Function *function) {
    if (!value || !function) {
        return R_InvalidArgument;
    }
    ValueDefaults(value);
    value->TypeInfo = &g_TheFunctionTypeInfo;
    value->v.Function = function;
    return R_OK;
}

int ValueMakeBuiltinFn(struct Value *value, struct BuiltinFn *builtinFn) {
    if (!value || !builtinFn) {
        return R_InvalidArgument;
    }
    ValueDefaults(value);
    value->TypeInfo = &g_TheBuiltinFnTypeInfo;
    value->IsBuiltInFn = 1;
    value->v.BuiltinFn = builtinFn;
    return R_OK;
}

char *ValueToString(struct Value *value) {
    /* TODO: Find better way to do ToString. */
    char buf[80];
    if (value->IsSymbol) {
        return ValueToString(value->v.Symbol->Value);
    }
    switch (value->TypeInfo->Type) {
        default:
            if (&g_TheNilValue == value) {
                return strdup("nil");
            }
            return strdup(value->TypeInfo->TypeName);
        case TypeString:
            return strdup(value->v.String->CString);
        case TypeBoolean:
            if (&g_TheTrueValue == value) {
                return strdup("true");
            }
            if (&g_TheFalseValue == value) {
                return strdup("false");
            }
            return strdup("<bad boolean>"); /* ??? how does this happen? */
        case TypeInteger:
            snprintf(buf, 80, "%d", value->v.Integer);
            return strdup(buf);
        case TypeReal:
            snprintf(buf, 80, "%f", value->v.Real);
            return strdup(buf);
    }
    
}
