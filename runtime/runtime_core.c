#include "runtime_core.h"
#include "globals.h"
#include "result.h"
#include "symbol_table.h"

#include "helpers/strings.h"

#include <stdio.h>

BuiltinFnProc_t RT_string;
BuiltinFnProc_t RT_print;
BuiltinFnProc_t RT_println;
BuiltinFnProc_t RT_type;

struct SrcLoc srcLoc = {"<runtime_core.c>", -1, -1};

char *ToString(struct Value *value) {
    /* TODO: Find better way to do ToString. */
    char buf[80];

    switch (value->TypeInfo->Type) {
        default:
            if (&g_TheNilValue == value) {
                return "nil";
            }
            return value->TypeInfo->TypeName;
        case TypeString:
            return value->v.String->CString;
        case TypeBoolean:
            if (&g_TheTrueValue == value) {
                return "true";
            }
            if (&g_TheFalseValue == value) {
                return "false";
            }
            return "<bad boolean>"; /* ??? how does this happen? */
        case TypeInteger:
            snprintf(buf, 80, "%d", value->v.Integer);
            return strdup(buf);
        case TypeReal:
            snprintf(buf, 80, "%f", value->v.Real);
            return strdup(buf);
    }
    
}

struct Value *_rt_print(unsigned int argc, struct Value **argv) {
    unsigned int i;
    char *s;
    for (i = 0; i < argc; ++i) {
        s = ToString(argv[i]);
        printf("%s", s);
        if (i + 1 < argc) {
            printf(" ");
        }
    }
    return &g_TheNilValue;
}

struct Value *_rt_println(unsigned int argc, struct Value **argv) {
    struct Value *value = _rt_print(argc, argv);
    printf("\n");
    return value;
}

struct Value *_rt_string(unsigned int argc, struct Value **argv) {
    struct Value *value;
    char *string = strdup(ToString(argv[0]));
    value = ValueAlloc();
    ValueMakeLLString(value, string);
    return value;
}

struct Value *_rt_type(unsigned int argc, struct Value **argv) {
    struct Value *value;
    char *type = strdup(argv[0]->TypeInfo->TypeName);
    value = ValueAlloc();
    ValueMakeLLString(value, type);
    return value;
}

int RegisterRuntime_core(struct LittleLangMachine *llm) {
    struct Value *value;
    struct BuiltinFn *fn;

    RT_print = _rt_print;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "print", 0, 1, RT_print);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(llm->GlobalScope, value, fn->Name, 0, srcLoc);

    RT_println = _rt_println;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "println", 0, 1, RT_println);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(llm->GlobalScope, value, fn->Name, 0, srcLoc);

    RT_string = _rt_string;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "string", 1, 0, RT_string);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(llm->GlobalScope, value, fn->Name, 0, srcLoc);

    RT_type = _rt_type;
    value = ValueAlloc();
    BuiltinFnMake(&fn, "type", 1, 0, RT_type);
    ValueMakeBuiltinFn(value, fn);
    SymbolTableInsert(llm->GlobalScope, value, fn->Name, 0, srcLoc);
    return R_OK;
}
