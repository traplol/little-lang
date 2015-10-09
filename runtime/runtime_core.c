#include "runtime_core.h"
#include "globals.h"
#include "result.h"
#include "symbol_table.h"

#include "helpers/strings.h"
#include "interpreter.h"
#include "helpers/macro_helpers.h"

#include <stdio.h>

BuiltinFnProc_t RT_string;
BuiltinFnProc_t RT_print;
BuiltinFnProc_t RT_println;
BuiltinFnProc_t RT_type;

static struct SrcLoc srcLoc = {"<runtime_core.c>", -1, -1};

struct Value *_rt_print(struct Module *module, unsigned int argc, struct Value **argv) {
    unsigned int i;
    struct Value *str;
    for (i = 0; i < argc; ++i) {
        str = InterpreterDispatchMethod(module, argv[i], "__str__", 0, NULL, srcLoc);
        if (&g_TheStringTypeInfo == str->TypeInfo) {
            printf("%s", str->v.String->CString);
        }
        if (i + 1 < argc) {
            printf(" ");
        }
    }
    return &g_TheNilValue;
}

struct Value *_rt_println(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *value = RT_print(module, argc, argv);
    printf("\n");
    return value;
}

struct Value *_rt_string(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *str = InterpreterDispatchMethod(module, argv[0], "__str__", 0, NULL, srcLoc);
    return str;
}

struct Value *_rt_type(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *value;
    char *type = argv[0]->TypeInfo->TypeName;
    ValueMakeLLString(&value, type);
    return value;
}

#define GLOBAL_FUNCTION_INSERT(name, numArgs, isVarArgs)                \
    do {                                                                \
        struct Value *func;                                             \
        struct BuiltinFn *fn;                                           \
        GLUE2(RT_, name) = GLUE2(_rt_, name);                           \
        func = ValueAllocNoGC();                                        \
        BuiltinFnMake(&fn, XSTR(name), numArgs, isVarArgs, GLUE2(RT_, name)); \
        ValueMakeBuiltinFn(func, fn);                                  \
        SymbolTableInsert(g_TheGlobalScope, func, fn->Name, 0, srcLoc); \
    } while (0)

int RegisterRuntime_core(void) {
    GLOBAL_FUNCTION_INSERT(print, 0, 1);
    GLOBAL_FUNCTION_INSERT(println, 0, 1);
    GLOBAL_FUNCTION_INSERT(string, 1, 0);
    GLOBAL_FUNCTION_INSERT(type, 1, 0);
    return R_OK;
}
