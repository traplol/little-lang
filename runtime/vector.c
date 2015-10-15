#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "interpreter.h"
#include "helpers/macro_helpers.h"
#include "runtime/string.h"

#include "result.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static struct SrcLoc srcLoc = {"vector.c", -1, -1};

static struct Value *rt_Vector_length(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *result, *self = argv[0];
    ValueMakeInteger(&result, self->v.Vector->Length);
    return result;
}

static struct Value *rt_Vector_new(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    unsigned int cap;
    if (argc > 1) {
        cap = argv[1]->v.Integer;
    }
    else {
        cap = 4;
    }
    self->TypeInfo = &g_TheVectorTypeInfo;
    self->IsPassByReference = 1;
    self->v.Vector = calloc(sizeof *self->v.Vector, 1);
    LLVectorMake(self->v.Vector, cap);
    return &g_TheNilValue;
}

static struct Value *rt_Vector___index__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *idx = argv[1];
    struct Value *ptrToValue;
    int i;
    if (&g_TheIntegerTypeInfo != idx->TypeInfo) {
        printf("%s.__idx__ only accepts Integers\n", self->TypeInfo->TypeName);
        return &g_TheNilValue;
    }
    i = idx->v.Integer;
    /* TODO: Negative indices? */
    if ((unsigned)i >= self->v.Vector->Length) {
        return &g_TheNilValue;
    }
    ptrToValue = ValueAlloc();
    ptrToValue->IsPtrToValue = 1;
    ptrToValue->v.PtrToValue = &(self->v.Vector->Values[i]);
    return ptrToValue;
}

static struct Value *rt_Vector_push_back(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    LLVectorAppendValue(self->v.Vector, other);
    return other;
}

static struct Value *rt_Vector___lshift__(struct Module *module, unsigned int argc, struct Value **argv) {
    return rt_Vector_push_back(module, argc, argv);
}

static struct Value *rt_Vector___str__(struct Module *module, unsigned int argc, struct Value **argv) {
    unsigned int i;
    struct Value *close, *sep, *other, *string, *self = argv[0];
    struct LLVector *v = self->v.Vector;
    struct Value *strArgv[2];
    ValueMakeLLStringWithCString(&string, "[");
    ValueMakeLLStringWithCString(&close, "]");
    ValueMakeLLStringWithCString(&sep, ", ");
    for (i = 0; i < v->Length; ++i) {
        other = InterpreterDispatchMethod(module, v->Values[i], "__str__", 0, NULL, srcLoc);
        strArgv[0] = string;
        strArgv[1] = other;
        string = RT_String_Concat(module, 2, strArgv);

        if (i + 1 < v->Length) {
            strArgv[0] = string;
            strArgv[1] = sep;
            string = RT_String_Concat(module, 2, strArgv);
        }
    }
    strArgv[0] = string;
    strArgv[1] = close;
    string = RT_String_Concat(module, 2, strArgv);
    return string;
}

static struct Value *rt_Vector___dbg__(struct Module *module, unsigned int argc, struct Value **argv) {
    return rt_Vector___str__(module, argc, argv);
}


#define VECTOR_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Vector_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheVectorTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Vector_RegisterBuiltins(void) {
    VECTOR_METHOD_INSERT(__str__, 1, 0);
    VECTOR_METHOD_INSERT(__dbg__, 1, 0);
    VECTOR_METHOD_INSERT(__index__, 2, 0);
    VECTOR_METHOD_INSERT(__lshift__, 2, 0);
    VECTOR_METHOD_INSERT(push_back, 2, 0);
    VECTOR_METHOD_INSERT(length, 1, 0);
    VECTOR_METHOD_INSERT(new, 1, 1);
    return R_OK;
}
