#include "registrar.h"
#include "result.h"

#include "runtime/runtime_core.h"
#include "runtime/string.h"
#include "runtime/integer.h"
#include "runtime/real.h"
#include "runtime/boolean.h"

#include <stdlib.h>

int RegisterRuntimes(void) {
    RegisterRuntime_core();
    RT_String_RegisterBuiltins();
    RT_Integer_RegisterBuiltins();
    RT_Real_RegisterBuiltins();
    RT_Boolean_RegisterBuiltins();
    return R_OK;
}

int FunctionMaker(struct Value **out_value, char *name, unsigned int numArgs, int isVarArgs, BuiltinFnProc_t proc) {
    struct Value *value;
    struct BuiltinFn *fn;
    if (!out_value || !name || !proc) {
        *out_value = NULL;
        return R_InvalidArgument;
    }
    value = ValueAllocNoGC();
    BuiltinFnMake(&fn, name, numArgs, isVarArgs, proc);
    ValueMakeBuiltinFn(value, fn);
    *out_value = value;
    return R_OK;
}
