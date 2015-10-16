#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "interpreter.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"
#include "value.h"

#include "result.h"

#include <stdlib.h>

static struct SrcLoc srcLoc = {"function.c", -1, -1};

static struct Value *rt_Function___str__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    ValueMakeLLStringWithCString(&out, self->v.Function->Name);
    return out;
}
static struct Value *rt_Function___dbg__(struct Module *module, unsigned int argc, struct Value **argv) {
    return rt_Function___str__(module, argc, argv);
}

#define FUNCTION_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Function_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheFunctionTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Function_RegisterBuiltins(void) {
    FUNCTION_METHOD_INSERT(__str__, 1, 0);
    FUNCTION_METHOD_INSERT(__dbg__, 1, 0);
    return R_OK;
}
