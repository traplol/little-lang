#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"

#include "result.h"

static struct SrcLoc srcLoc = {"boolean.c", -1, -1};

static struct Value *rt_Boolean___eq__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (self == other) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Boolean___not__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    if (&g_TheFalseValue == self) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Boolean___str__(unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    char *s;
    if (&g_TheTrueValue == self) {
        s = strdup("true");
    }
    else {
        s = strdup("false");
    }
    ValueMakeLLString(&out, s);
    return out;
}
static struct Value *rt_Boolean___hash__(unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    int n = &g_TheTrueValue == self;
    ValueMakeInteger(&out, n);
    return out;
}
static struct Value *rt_Boolean___dbg__(unsigned int argc, struct Value **argv) {
    return rt_Boolean___str__(argc, argv);
}

#define BOOLEAN_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Boolean_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheBooleanTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Boolean_RegisterBuiltins(void) {
    BOOLEAN_METHOD_INSERT(__eq__, 2, 0);
    BOOLEAN_METHOD_INSERT(__not__, 1, 0);
    BOOLEAN_METHOD_INSERT(__str__, 1, 0);
    BOOLEAN_METHOD_INSERT(__hash__, 1, 0);
    BOOLEAN_METHOD_INSERT(__dbg__, 1, 0);
    return R_OK;
}
