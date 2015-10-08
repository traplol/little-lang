#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"

#include "result.h"

static struct SrcLoc srcLoc = {"string.c", -1, -1};

static struct Value *rt_String__add__(unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    struct Value *other = argv[1];
    char *s, *l, *r;
    l = self->v.String->CString;
    r = other->v.String->CString;
    s = str_cat(l, r);
    ValueMakeLLString(&out, s);
    return out;
}
static struct Value *rt_String__eq__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    char *l, *r;
    if (self == other) {
        return &g_TheTrueValue;
    }
    l = self->v.String->CString;
    r = other->v.String->CString;
    for (; *l && *r; ++l, ++r) {
        if (*l != *r) {
            return &g_TheFalseValue;
        }
    }
    return &g_TheTrueValue;
}
static struct Value *rt_String__str__(unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    ValueMakeLLString(&out, self->v.String->CString);
    return out;
}
static struct Value *rt_String__hash__(unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    int n = string_hash(self->v.String->CString);
    ValueMakeInteger(&out, n);
    return out;
}
static struct Value *rt_String__dbg__(unsigned int argc, struct Value **argv) {
    /* TODO: Implement the escapes. */
    return rt_String__str__(argc, argv);
}

#define STRING_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_String, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheStringTypeInfo, method, srcLoc);     \
    } while (0)

int RT_String_RegisterBuiltins(void) {
    STRING_METHOD_INSERT(__add__, 2, 0);
    STRING_METHOD_INSERT(__eq__, 2, 0);
    STRING_METHOD_INSERT(__str__, 1, 0);
    STRING_METHOD_INSERT(__hash__, 1, 0);
    STRING_METHOD_INSERT(__dbg__, 1, 0);
    return R_OK;
}
