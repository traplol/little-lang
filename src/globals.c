#include "globals.h"
#include "value.h"
#include "result.h"

#define RETURN_ON_FAIL(r)                       \
    do {                                        \
        if (R_OK != (r)) {                      \
            return r;                           \
        }                                       \
    } while (0)

struct Value g_TheTrueValue;
struct Value g_TheFalseValue;
struct Value g_TheNilValue;

struct TypeInfo g_TheBaseObjectTypeInfo;
struct TypeInfo g_TheIntegerTypeInfo;
struct TypeInfo g_TheRealTypeInfo;
struct TypeInfo g_TheStringTypeInfo;
struct TypeInfo g_TheBooleanTypeInfo;

int GlobalsInitTypeInfos(void) {
    int result;
    result = TypeInfoMake(&g_TheBaseObjectTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "Object");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheIntegerTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "Integer");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheRealTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "Real");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheStringTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "String");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheBooleanTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "Boolean");
    RETURN_ON_FAIL(result);

    return R_OK;
}

int GlobalsInitSingletonValues(void) {
    int result;
    result = ValueMake(&g_TheTrueValue, &g_TheBooleanTypeInfo, 0, 0);
    RETURN_ON_FAIL(result);

    result = ValueMake(&g_TheFalseValue, &g_TheBooleanTypeInfo, 0, 0);
    RETURN_ON_FAIL(result);

    result = ValueMake(&g_TheNilValue, &g_TheBaseObjectTypeInfo, 0, 0);
    RETURN_ON_FAIL(result);

    return R_OK;
}

static int __globalValuesInitted = 0;
int GlobalsInit(void) {
    int result;
    if (__globalValuesInitted) {
        return R_GlobalsAlreadyInitted;
    }
    result = GlobalsInitTypeInfos();
    RETURN_ON_FAIL(result);

    result = GlobalsInitSingletonValues();
    RETURN_ON_FAIL(result);
    __globalValuesInitted = 1;
    return R_OK;
}
