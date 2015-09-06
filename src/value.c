#include "value.h"
#include "result.h"

#include "string.h"

int ValueMake(struct Value *value, struct TypeInfo *typeInfo, void *data, unsigned int dataSize) {
    if (!value || !typeInfo) {
        return R_InvalidArgument;
    }
    value->TypeInfo = typeInfo;
    value->IsBuiltInFn = 0;
    if (data && dataSize) {
        memcpy(value->v.__ptrsize, data, dataSize);
    }
    return R_OK;
}


int ValueMakeBuiltinFn(struct Value *value, struct TypeInfo *typeInfo, BuiltinFn_t builtinFn) {
    int result = ValueMake(value, typeInfo, 0, 0);
    if (R_OK != result) {
        return result;
    }
    value->IsBuiltInFn = 1;
    value->v.BuiltinFn = builtinFn;
    return R_OK;
}
