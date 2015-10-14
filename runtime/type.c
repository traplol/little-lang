#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"
#include "interpreter.h"

#include "result.h"

static struct SrcLoc srcLoc = {"type.c", -1, -1};

static struct Value *rt_Type_new(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *value, *func;

    if (TypeUserObject == self->v.MetaTypeInfo->Type) {
        value = InterpreterBuildObjectWithDefaults(module, self->v.MetaTypeInfo);
        if (R_OK == TypeInfoHasMethod(value->TypeInfo, "new")) {
            /* TODO: maybe validate argc and argv here? */
            InterpreterDispatchMethod(module, value, "new", argc-1, argv+1, srcLoc);
        }
    }
    else {
        value = ValueAlloc();
        TypeInfoLookupMethod(self->v.MetaTypeInfo, "new", &func);
        if (func) {
            argv[0] = value;
            if (func->IsBuiltInFn) {
                InterpreterDoCallBuiltinFn(module, func, argc, argv, srcLoc);
            }
            else {
                InterpreterDoCallFunction(module, func, argc, argv, srcLoc);
            }
        }
    }
    
    return value;
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
    return R_OK;
}
