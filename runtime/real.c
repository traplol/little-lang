
#include "registrar.h"
#include "type_info.h"
#include "globals.h"
#include "helpers/macro_helpers.h"
#include "helpers/strings.h"

#include "result.h"

#include <math.h>
#include <stdio.h>

static struct SrcLoc srcLoc = {"real.c", -1, -1};

#define IS_INTEGER(v) ((v)->TypeInfo == &g_TheIntegerTypeInfo)
#define IS_REAL(v) ((v)->TypeInfo == &g_TheRealTypeInfo)
#define IS_NUMERIC(v) (IS_INTEGER(v) || IS_REAL(v))
#define REAL_EPSILON (1e-16)

static struct Value *rt_Real___add__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    double r;
    if (IS_INTEGER(other)) {
        r = self->v.Real + other->v.Integer;
        ValueMakeReal(&result, r);
        return result;
    }
    if (IS_REAL(other)) {
        r = self->v.Real + other->v.Real;
        ValueMakeReal(&result, r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Real___sub__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    double r;
    if (IS_INTEGER(other)) {
        r = self->v.Real - other->v.Integer;
        ValueMakeReal(&result, r);
        return result;
    }
    if (IS_REAL(other)) {
        r = self->v.Real - other->v.Real;
        ValueMakeReal(&result, r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Real___mul__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    double r;
    if (IS_INTEGER(other)) {
        r = self->v.Real * other->v.Integer;
        ValueMakeReal(&result, r);
        return result;
    }
    if (IS_REAL(other)) {
        r = self->v.Real * other->v.Real;
        ValueMakeReal(&result, r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Real___div__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    double r;
    if (IS_INTEGER(other)) {
        r = self->v.Real / other->v.Integer;
        ValueMakeReal(&result, r);
        return result;
    }
    if (IS_REAL(other)) {
        r = self->v.Real / other->v.Real;
        ValueMakeReal(&result, r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Real___mod__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    double r;
    if (IS_INTEGER(other)) {
        r = fmod(self->v.Real, other->v.Integer);
        ValueMakeReal(&result, r);
        return result;
    }
    if (IS_REAL(other)) {
        r = fmod(self->v.Real, other->v.Real);
        ValueMakeReal(&result, r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Real___pow__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    struct Value *result;
    double r;
    if (IS_INTEGER(other)) {
        r = pow(self->v.Real, other->v.Integer);
        ValueMakeReal(&result, r);
        return result;
    }
    if (IS_REAL(other)) {
        r = pow(self->v.Real, other->v.Real);
        ValueMakeReal(&result, r);
        return result;
    }
    return &g_TheNilValue;
}
static struct Value *rt_Real___neg__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *result;
    ValueMakeReal(&result, -self->v.Real);
    return result;
}
static struct Value *rt_Real___eq__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    double diff;
    if (IS_INTEGER(other)) {
        diff = self->v.Real - other->v.Integer;
        if (0.0 == diff) {
            return &g_TheTrueValue;
        }
    }
    if (IS_REAL(other)) {
        diff = self->v.Real - other->v.Real;
        if (fabs(diff) <= REAL_EPSILON) {
            return &g_TheTrueValue;
        }
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Real___lt__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (IS_INTEGER(other)) {
        if (self->v.Real - other->v.Integer < 0) {
            return &g_TheTrueValue;
        }
    }
    if (IS_REAL(other)) {
        if (self->v.Real - other->v.Real < 0) {
            return &g_TheTrueValue;
        }
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Real___gt__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    struct Value *other = argv[1];
    if (IS_INTEGER(other)) {
        if (self->v.Real - other->v.Integer > 0) {
            return &g_TheTrueValue;
        }
    }
    if (IS_REAL(other)) {
        if (self->v.Real - other->v.Real > 0) {
            return &g_TheTrueValue;
        }
    }
    return &g_TheFalseValue;
}
static struct Value *rt_Real___str__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *out, *self = argv[0];
    char *s, buf[80];
    snprintf(buf, sizeof(buf)/sizeof(*buf), "%f", self->v.Real);
    s = strdup(buf);
    ValueMakeLLStringWithCString(&out, s);
    return out;
}
static struct Value *rt_Real___hash__(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *result, *self = argv[0];
    union {
        uint64_t whole;
        struct {
            uint32_t high;
            uint32_t low;
        } BE;
        struct {
            uint32_t low;
            uint32_t high;
        } LE;
    } bits;
    bits.whole = self->v.RealToIntBits;
    int i = (bits.LE.high ^ bits.LE.low) & 0x7fffffff;
    ValueMakeInteger(&result, i);
    return result;
}
static struct Value *rt_Real___dbg__(struct Module *module, unsigned int argc, struct Value **argv) {
    return rt_Real___str__(module, argc, argv);
}

static struct Value *rt_Real_is_nan(struct Module *module, unsigned int argc, struct Value **argv) {
    struct Value *self = argv[0];
    if (isnan(self->v.Real)) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}

#define REAL_METHOD_INSERT(name, numArgs, isVarArgs)                  \
    do {                                                                \
        struct Value *method;                                           \
        int result = FunctionMaker(&method, XSTR(name), numArgs, isVarArgs, GLUE2(rt_Real_, name)); \
        if (R_OK != result) {                                           \
            return result;                                              \
        }                                                               \
        TypeInfoInsertMethod(&g_TheRealTypeInfo, method, srcLoc);     \
    } while (0)

int RT_Real_RegisterBuiltins(void) {
    REAL_METHOD_INSERT(__add__, 2, 0);
    REAL_METHOD_INSERT(__sub__, 2, 0);
    REAL_METHOD_INSERT(__mul__, 2, 0);
    REAL_METHOD_INSERT(__div__, 2, 0);
    REAL_METHOD_INSERT(__mod__, 2, 0);
    REAL_METHOD_INSERT(__pow__, 2, 0);
    REAL_METHOD_INSERT(__eq__, 2, 0);
    REAL_METHOD_INSERT(__lt__, 2, 0);
    REAL_METHOD_INSERT(__gt__, 2, 0);
    REAL_METHOD_INSERT(__neg__, 1, 0);
    REAL_METHOD_INSERT(__str__, 1, 0);
    REAL_METHOD_INSERT(__hash__, 1, 0);
    REAL_METHOD_INSERT(__dbg__, 1, 0);
    REAL_METHOD_INSERT(is_nan, 1, 0);
    return R_OK;
}
