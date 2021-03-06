#include "runtime_core.h"
#include "globals.h"
#include "result.h"
#include "symbol_table.h"

#include "runtime/gc.h"

#include "helpers/strings.h"
#include "interpreter.h"
#include "helpers/macro_helpers.h"

#include <stdio.h>

BuiltinFnProc_t RT_string;
BuiltinFnProc_t RT_print;
BuiltinFnProc_t RT_println;
BuiltinFnProc_t RT_type;
BuiltinFnProc_t RT_hash;
BuiltinFnProc_t RT_dbg;

BuiltinFnProc_t RT___gc_dump;
BuiltinFnProc_t RT___gc_reachable;
BuiltinFnProc_t RT___gc_disable;
BuiltinFnProc_t RT___gc_enable;
BuiltinFnProc_t RT___gc_is_disabled;

static struct SrcLoc srcLoc = {"<runtime_core.c>", -1, -1};

static struct Value *_rt_print(struct Module *module, unsigned int argc, struct Value **argv) {
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

static struct Value *_rt_println(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *value = RT_print(module, argc, argv);
    printf("\n");
    return value;
}

static struct Value *_rt_string(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *str = InterpreterDispatchMethod(module, argv[0], "__str__", 0, NULL, srcLoc);
    return str;
}

static struct Value *_rt_type(struct Module *module, unsigned int argc, struct Value **argv) {
    char *typeName = argv[0]->TypeInfo->TypeName;
    struct Symbol *symbol;
    SymbolTableFindNearest(module->CurrentScope, typeName, &symbol);
    if (!symbol) {
        SymbolTableFindLocal(g_TheGlobalScope, typeName, &symbol);
    }
    return symbol->Value;
}

static struct Value *_rt_hash(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *hash = InterpreterDispatchMethod(module, argv[0], "__hash__", 0, NULL, srcLoc);
    return hash;
}

static struct Value *_rt_dbg(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *dbg = InterpreterDispatchMethod(module, argv[0], "__dbg__", 0, NULL, srcLoc);
    return dbg;
}

static struct Value *_rt___gc_dump(struct Module *module, unsigned int argc, struct Value **argv) {
    GC_Dump();
    return &g_TheNilValue;
}

static struct Value *_rt___gc_reachable(struct Module *module, unsigned int argc, struct Value **argv) {
    GC_DumpReachable();
    return &g_TheNilValue;
}

static inline void no_gc_msg(void) {
    printf("GC was disabled at compile time, this does nothing.\n");
}
static struct Value *_rt___gc_disable(struct Module *module, unsigned int argc, struct Value **argv) {
#ifdef NO_GC
    no_gc_msg();
#else
    GC_Disable();
#endif
    return &g_TheNilValue;
}
static struct Value *_rt___gc_enable(struct Module *module, unsigned int argc, struct Value **argv) {
#ifdef NO_GC
    no_gc_msg();
#else
    GC_Enable();
#endif
    return &g_TheNilValue;
}

static struct Value *_rt___gc_is_disabled(struct Module *module, unsigned int argc, struct Value **argv) {
    if (GC_isDisabled()) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
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
    GLOBAL_FUNCTION_INSERT(hash, 1, 0);
    GLOBAL_FUNCTION_INSERT(dbg, 1, 0);

    GLOBAL_FUNCTION_INSERT(__gc_dump, 0, 0);
    GLOBAL_FUNCTION_INSERT(__gc_reachable, 0, 0);
    GLOBAL_FUNCTION_INSERT(__gc_enable, 0, 0);
    GLOBAL_FUNCTION_INSERT(__gc_disable, 0, 0);
    GLOBAL_FUNCTION_INSERT(__gc_is_disabled, 0, 0);
    return R_OK;
}
