#include "runtime_core.h"
#include "globals.h"
#include "result.h"
#include "symbol_table.h"

#include "helpers/strings.h"
#include "interpreter.h"

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

int RegisterRuntime_core(void) {
    struct Value *value;
    struct BuiltinFn *fn;

    RT_print = _rt_print;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "print", 0, 1, RT_print);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(g_TheGlobalScope, value, fn->Name, 0, srcLoc);

    RT_println = _rt_println;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "println", 0, 1, RT_println);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(g_TheGlobalScope, value, fn->Name, 0, srcLoc);

    RT_string = _rt_string;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "string", 1, 0, RT_string);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(g_TheGlobalScope, value, fn->Name, 0, srcLoc);

    RT_type = _rt_type;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "type", 1, 0, RT_type);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(g_TheGlobalScope, value, fn->Name, 0, srcLoc);
    return R_OK;
}
