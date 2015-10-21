#include "globals.h"
#include "value.h"
#include "type_table.h"
#include "symbol_table.h"
#include "result.h"

#include "runtime/literal_table.h"

#include "helpers/macro_helpers.h"

#include <stdlib.h>

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
struct TypeInfo g_TheTypeTypeInfo;
struct TypeInfo g_TheFunctionTypeInfo;
struct TypeInfo g_TheBuiltinFnTypeInfo;
struct TypeInfo g_TheIntegerTypeInfo;
struct TypeInfo g_TheRealTypeInfo;
struct TypeInfo g_TheStringTypeInfo;
struct TypeInfo g_TheBooleanTypeInfo;
struct TypeInfo g_TheVectorTypeInfo;

struct TypeTable g_TheGlobalTypeTable;

struct SymbolTable *g_TheGlobalScope;
struct SymbolTable g_TheUberScope;

static struct SrcLoc srcLoc = {"<globals.c>", -1, -1};

#define MAKE_TYPEINFO_AND_CONSTANT(name, type)                          \
    do {                                                                \
        struct TypeInfo *ti = &GLUE3(g_The, name, TypeInfo);            \
        struct Value *v;                                                \
        int result = TypeInfoMake(ti, type, &g_TheBaseObjectTypeInfo, XSTR(name)); \
        RETURN_ON_FAIL(result);                                         \
        ValueMakeType(&v, ti);                                          \
        result = SymbolTableInsert(g_TheGlobalScope, v, ti->TypeName, 0, srcLoc); \
        RETURN_ON_FAIL(result);                                         \
    } while (0)

static int GlobalsInitTypeInfos(void) {
    MAKE_TYPEINFO_AND_CONSTANT(BaseObject, TypeBaseObject);
    MAKE_TYPEINFO_AND_CONSTANT(Type, TypeType);
    MAKE_TYPEINFO_AND_CONSTANT(Function, TypeFunction);
    MAKE_TYPEINFO_AND_CONSTANT(BuiltinFn, TypeFunction);
    MAKE_TYPEINFO_AND_CONSTANT(Integer, TypeInteger);
    MAKE_TYPEINFO_AND_CONSTANT(Real, TypeReal);
    MAKE_TYPEINFO_AND_CONSTANT(String, TypeString);
    MAKE_TYPEINFO_AND_CONSTANT(Boolean, TypeBoolean);
    MAKE_TYPEINFO_AND_CONSTANT(Vector, TypeVector);
    return R_OK;
}

static int GlobalsInitSingletonValues(void) {
    int result;
    result = ValueMakeSingleton(&g_TheNilValue, &g_TheBaseObjectTypeInfo);
    RETURN_ON_FAIL(result);
    LiteralLookupTableInsert(&g_TheNilValue);

    result = ValueMakeSingleton(&g_TheTrueValue, &g_TheBooleanTypeInfo);
    RETURN_ON_FAIL(result);
    LiteralLookupTableInsert(&g_TheTrueValue);

    result = ValueMakeSingleton(&g_TheFalseValue, &g_TheBooleanTypeInfo);
    RETURN_ON_FAIL(result);
    LiteralLookupTableInsert(&g_TheFalseValue);
    return R_OK;
}

static int GlobalsInitGlobalScope(void) {
    int result;
    g_TheGlobalScope = calloc(sizeof *g_TheGlobalScope, 1);
    result = SymbolTableMakeGlobalScope(g_TheGlobalScope);
    g_TheUberScope.Child = g_TheGlobalScope;
    g_TheGlobalScope->Parent = &g_TheUberScope;
    return result;
}

static int GlobalsInitGlobalTypeInfo(void) {
    int result;
    result = TypeTableMake(&g_TheGlobalTypeTable, 53);
    RETURN_ON_FAIL(result);

    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheBaseObjectTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheFunctionTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheBuiltinFnTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheIntegerTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheRealTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheStringTypeInfo);
    RETURN_ON_FAIL(result);
    result = TypeTableInsert(&g_TheGlobalTypeTable, &g_TheBooleanTypeInfo);
    RETURN_ON_FAIL(result);
    return R_OK;
}

static int __globalValuesInitialized = 0;
int GlobalsInit(void) {
    int result;
    if (__globalValuesInitialized) {
        return R_GlobalsAlreadyInitted;
    }
    result = GlobalsInitGlobalScope();
    RETURN_ON_FAIL(result);

    result = GlobalsInitSingletonValues();
    RETURN_ON_FAIL(result);

    result = GlobalsInitTypeInfos();
    RETURN_ON_FAIL(result);

    result = GlobalsInitGlobalTypeInfo();
    RETURN_ON_FAIL(result);

    __globalValuesInitialized = 1;
    return R_OK;
}

int GlobalsDenit(void) {
    int result;
    if (!__globalValuesInitialized) {
        return R_OperationFailed;
    }
    g_TheUberScope.Child = NULL;
    
    result = SymbolTableFree(g_TheGlobalScope);
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
