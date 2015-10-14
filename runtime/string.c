#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "interpreter.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"

#include "result.h"

#include <string.h>

static struct SrcLoc srcLoc = {"string.c", -1, -1};

BuiltinFnProc_t RT_String_Concat;

static struct Value *rt_String___add__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    struct Value *other = argv[1];
    char *s, *l, *r;
    l = self->v.String->CString;
    r = other->v.String->CString;
    s = str_cat(l, r);
    ValueMakeLLString(&out, s);
    return out;
}
static struct Value *rt_String___eq__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    char *l, *r;
    if (self == other) {
        return &g_TheTrueValue;
    }
    l = self->v.String->CString;
    r = other->v.String->CString;
    if (0 == strcmp(l, r)) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
static struct Value *rt_String___str__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    ValueMakeLLString(&out, self->v.String->CString);
    return out;
}
static struct Value *rt_String___hash__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    int n = string_hash(self->v.String->CString);
    ValueMakeInteger(&out, n);
    return out;
}
static struct Value *rt_String___dbg__(struct Module *module, unsigned int argc, struct Value **argv) {
    /* TODO: Implement the escapes. */
    return rt_String___str__(module, argc, argv);
}

static struct Value *rt_String_length(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *result, *self = argv[0];
    ValueMakeInteger(&result, self->v.String->Length);
    return result;
}

static struct Value *rt_String_new(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *arg = argv[1];
    struct Value *s = InterpreterDispatchMethod(module, arg, "__str__", 0, NULL, srcLoc);
    self->v.String = s->v.String;
    return &g_TheNilValue;
}

#define STRING_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_String_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheStringTypeInfo, method, srcLoc);     \
    } while (0)

int RT_String_RegisterBuiltins(void) {
    RT_String_Concat = rt_String___add__;
    STRING_METHOD_INSERT(__add__, 2, 0);
    STRING_METHOD_INSERT(__eq__, 2, 0);
    STRING_METHOD_INSERT(__str__, 1, 0);
    STRING_METHOD_INSERT(__hash__, 1, 0);
    STRING_METHOD_INSERT(__dbg__, 1, 0);
    STRING_METHOD_INSERT(length, 1, 0);
    STRING_METHOD_INSERT(new, 2, 0);
    return R_OK;
}
