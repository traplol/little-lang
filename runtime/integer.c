#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"

#include "result.h"

#include <math.h>
#include <stdio.h>

static struct SrcLoc srcLoc = {"integer.c", -1, -1};

#define IS_INTEGER(v) ((v)->TypeInfo == &g_TheIntegerTypeInfo)
#define IS_REAL(v) ((v)->TypeInfo == &g_TheRealTypeInfo)
#define IS_NUMERIC(v) (IS_INTEGER(v) || IS_REAL(v))

static struct Value *rt_Integer___add__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer + other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    if (IS_REAL(other)) {
        r.r = self->v.Integer + other->v.Real;
        ValueMakeReal(&result, r.r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___sub__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer - other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    if (IS_REAL(other)) {
        r.r = self->v.Integer - other->v.Real;
        ValueMakeReal(&result, r.r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___mul__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer * other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    if (IS_REAL(other)) {
        r.r = self->v.Integer * other->v.Real;
        ValueMakeReal(&result, r.r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___div__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer / other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    if (IS_REAL(other)) {
        r.r = self->v.Integer / other->v.Real;
        ValueMakeReal(&result, r.r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___mod__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer % other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    if (IS_REAL(other)) {
        r.r = fmod(self->v.Integer, other->v.Real);
        ValueMakeReal(&result, r.r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___and__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer & other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___or__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer | other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___xor__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer ^ other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___pow__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = pow(self->v.Integer, other->v.Integer);
        ValueMakeInteger(&result, r.i);
        return result;
    }
    if (IS_REAL(other)) {
        r.r = pow(self->v.Integer, other->v.Real);
        ValueMakeReal(&result, r.r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___neg__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *result;
    ValueMakeInteger(&result, -self->v.Integer);
    return result;
}
static struct Value *rt_Integer___pos__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    return self;
}
static struct Value *rt_Integer___lshift__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer << other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___rshift__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    union {
        double r;
        int i;
    } r;
    if (IS_INTEGER(other)) {
        r.i = self->v.Integer >> other->v.Integer;
        ValueMakeInteger(&result, r.i);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Integer___eq__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (IS_INTEGER(other)) {
        if (self->v.Integer == other->v.Integer) {
            return &g_TheTrueValue;
        }
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Integer___lt__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (IS_INTEGER(other)) {
        if (self->v.Integer < other->v.Integer) {
            return &g_TheTrueValue;
        }
    }
    if (IS_REAL(other)) {
        if (self->v.Integer < other->v.Real) {
            return &g_TheTrueValue;
        }
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Integer___gt__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (IS_INTEGER(other)) {
        if (self->v.Integer > other->v.Integer) {
            return &g_TheTrueValue;
        }
    }
    if (IS_REAL(other)) {
        if (self->v.Integer > other->v.Real) {
            return &g_TheTrueValue;
        }
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Integer___str__(unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    char *s, buf[80];
    snprintf(buf, sizeof(buf)/sizeof(*buf), "%d", self->v.Integer);
    s = strdup(buf);
    ValueMakeLLString(&out, s);
    return out;
}
static struct Value *rt_Integer___hash__(unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    return self;
}
static struct Value *rt_Integer___dbg__(unsigned int argc, struct Value **argv) {
    return rt_Integer___str__(argc, argv);
}

#define INTEGER_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Integer_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheIntegerTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Integer_RegisterBuiltins(void) {
    INTEGER_METHOD_INSERT(__add__, 2, 0);
    INTEGER_METHOD_INSERT(__sub__, 2, 0);
    INTEGER_METHOD_INSERT(__mul__, 2, 0);
    INTEGER_METHOD_INSERT(__div__, 2, 0);
    INTEGER_METHOD_INSERT(__mod__, 2, 0);
    INTEGER_METHOD_INSERT(__and__, 2, 0);
    INTEGER_METHOD_INSERT(__or__, 2, 0);
    INTEGER_METHOD_INSERT(__xor__, 2, 0);
    INTEGER_METHOD_INSERT(__pow__, 2, 0);
    INTEGER_METHOD_INSERT(__lshift__, 2, 0);
    INTEGER_METHOD_INSERT(__rshift__, 2, 0);
    INTEGER_METHOD_INSERT(__eq__, 2, 0);
    INTEGER_METHOD_INSERT(__lt__, 2, 0);
    INTEGER_METHOD_INSERT(__gt__, 2, 0);
    INTEGER_METHOD_INSERT(__neg__, 1, 0);
    INTEGER_METHOD_INSERT(__pos__, 1, 0);
    INTEGER_METHOD_INSERT(__str__, 1, 0);
    INTEGER_METHOD_INSERT(__hash__, 1, 0);
    INTEGER_METHOD_INSERT(__dbg__, 1, 0);
    return R_OK;
}
