#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"
#include "interpreter.h"

#include "result.h"

static struct SrcLoc srcLoc = {"type.c", -1, -1};

static inline unsigned int isPBR(struct TypeInfo *typeInfo) {
    switch (typeInfo->Type) {
    default:
        return 1;
    case TypeInteger:
    case TypeReal:
        return 0;
    }
}

static struct Value *rt_Type_new(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *value, *func;

    if (TypeUserObject == self->v.MetaTypeInfo->Type) {
        value = InterpreterBuildObjectWithDefaults(module, self->v.MetaTypeInfo);
    }
    else {
        value = ValueAlloc();
        value->TypeInfo = self->v.MetaTypeInfo;
        value->IsPassByReference = isPBR(value->TypeInfo);
    }
    TypeInfoLookupMethod(value->TypeInfo, "new", &func);
    if (func) {
        argv[0] = value;
        if (func->IsBuiltInFn) {
            InterpreterDoCallBuiltinFn(module, func, argc, argv, srcLoc);
        }
        else {
            InterpreterDoCallFunction(module, func, argc, argv, srcLoc);
        }
    }
    return value;
}

static struct Value *rt_Type___str__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *string;
    ValueMakeLLString(&string, self->v.MetaTypeInfo->TypeName);
    return string;
}

#define TYPE_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Type_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheTypeTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Type_RegisterBuiltins(void) {
    TYPE_METHOD_INSERT(new, 0, 1);
    TYPE_METHOD_INSERT(__str__, 1, 0);
    return R_OK;
}
