#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"

#include "result.h"

static struct SrcLoc srcLoc = {"object.c", -1, -1};

static struct Value *rt_Object__add__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__sub__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__mul__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__div__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__mod__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__and__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__or__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__xor__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__pow__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__neg__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__pos__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__lshift__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__rshift__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__not__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__eq__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (self == other) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Object__lt__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__gt__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__index__(unsigned int argc, struct Value **argv) {
    return &g_TheNilValue;
}
static struct Value *rt_Object__str__(unsigned int argc, struct Value **argv) {
    struct Value *result, *self = argv[0];
    ValueMakeLLString(&result, self->TypeInfo->TypeName);
    return result;
}
static struct Value *rt_Object__hash__(unsigned int argc, struct Value **argv) {
    struct Value *result, *self = argv[0];
    ValueMakeInteger(&result, (int)self);
    return result;
}
static struct Value *rt_Object__dbg__(unsigned int argc, struct Value **argv) {
    return rt_Object__str__(argc, argv);
}

#define OBJECT_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Object, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheBaseObjectTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Object_RegisterBuiltins(void) {
    OBJECT_METHOD_INSERT(__add__, 2, 0);
    OBJECT_METHOD_INSERT(__sub__, 2, 0);
    OBJECT_METHOD_INSERT(__mul__, 2, 0);
    OBJECT_METHOD_INSERT(__div__, 2, 0);
    OBJECT_METHOD_INSERT(__mod__, 2, 0);
    OBJECT_METHOD_INSERT(__and__, 2, 0);
    OBJECT_METHOD_INSERT(__or__, 2, 0);
    OBJECT_METHOD_INSERT(__xor__, 2, 0);
    OBJECT_METHOD_INSERT(__pow__, 2, 0);
    OBJECT_METHOD_INSERT(__lshift__, 2, 0);
    OBJECT_METHOD_INSERT(__rshift__, 2, 0);
    OBJECT_METHOD_INSERT(__eq__, 2, 0);
    OBJECT_METHOD_INSERT(__lt__, 2, 0);
    OBJECT_METHOD_INSERT(__gt__, 2, 0);
    OBJECT_METHOD_INSERT(__neg__, 1, 0);
    OBJECT_METHOD_INSERT(__not__, 1, 0);
    OBJECT_METHOD_INSERT(__pos__, 1, 0);
    OBJECT_METHOD_INSERT(__index__, 1, 0);
    OBJECT_METHOD_INSERT(__str__, 1, 0);
    OBJECT_METHOD_INSERT(__hash__, 1, 0);
    OBJECT_METHOD_INSERT(__dbg__, 1, 0);
    return R_OK;
}