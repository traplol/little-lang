#include "globals.h"
#include "value.h"
#include "symbol_table.h"
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
struct TypeInfo g_TheFunctionTypeInfo;
struct TypeInfo g_TheBuiltinFnTypeInfo;
struct TypeInfo g_TheIntegerTypeInfo;
struct TypeInfo g_TheRealTypeInfo;
struct TypeInfo g_TheStringTypeInfo;
struct TypeInfo g_TheBooleanTypeInfo;

struct SymbolTable g_TheGlobalScope;

int GlobalsInitTypeInfos(void) {
    int result;
    result = TypeInfoMake(&g_TheBaseObjectTypeInfo, TypeBaseObject, 0, "Object");
    RETURN_ON_FAIL(result);
    g_TheBaseObjectTypeInfo.DerivedFrom = &g_TheBaseObjectTypeInfo;

    result = TypeInfoMake(&g_TheFunctionTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "Function");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheBuiltinFnTypeInfo, TypeBaseObject, &g_TheBaseObjectTypeInfo, "BuiltinFn");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheIntegerTypeInfo, TypeInteger, &g_TheBaseObjectTypeInfo, "Integer");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheRealTypeInfo, TypeReal, &g_TheBaseObjectTypeInfo, "Real");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheStringTypeInfo, TypeString, &g_TheBaseObjectTypeInfo, "String");
    RETURN_ON_FAIL(result);

    result = TypeInfoMake(&g_TheBooleanTypeInfo, TypeBoolean, &g_TheBaseObjectTypeInfo, "Boolean");
    RETURN_ON_FAIL(result);

    return R_OK;
}

int GlobalsInitSingletonValues(void) {
    int result;
    result = ValueMakeObject(&g_TheTrueValue, &g_TheBooleanTypeInfo, 0, 0);
    RETURN_ON_FAIL(result);

    result = ValueMakeObject(&g_TheFalseValue, &g_TheBooleanTypeInfo, 0, 0);
    RETURN_ON_FAIL(result);

    result = ValueMakeObject(&g_TheNilValue, &g_TheBaseObjectTypeInfo, 0, 0);
    RETURN_ON_FAIL(result);

    return R_OK;
}

int GlobalsInitGlobalScope(void) {
    return SymbolTableMakeGlobalScope(&g_TheGlobalScope);
}

static int __globalValuesInitialized = 0;
int GlobalsInit(void) {
    int result;
    if (__globalValuesInitialized) {
        return R_GlobalsAlreadyInitted;
    }
    result = GlobalsInitTypeInfos();
    RETURN_ON_FAIL(result);

    result = GlobalsInitSingletonValues();
    RETURN_ON_FAIL(result);

    result = GlobalsInitGlobalScope();
    RETURN_ON_FAIL(result);

    __globalValuesInitialized = 1;
    return R_OK;
}

int GlobalsDenit(void) {
    int result;
    if (!__globalValuesInitialized) {
        return R_OperationFailed;
    }
    result = SymbolTableFree(&g_TheGlobalScope);
    RETURN_ON_FAIL(result);

    result = TypeInfoFree(&g_TheBaseObjectTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeInfoFree(&g_TheFunctionTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeInfoFree(&g_TheBuiltinFnTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeInfoFree(&g_TheIntegerTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeInfoFree(&g_TheRealTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeInfoFree(&g_TheStringTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeInfoFree(&g_TheBooleanTypeInfo);
    return result;
}
