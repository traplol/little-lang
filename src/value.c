#include "value.h"

#include "string.h"

int ValueMake(struct Value *value, struct TypeInfo *typeInfo, void *data, unsigned int dataSize) {
    if (!value || !typeInfo) {
        return -1;
    }
    value->TypeInfo = typeInfo;
    memcpy(value->v.__ptrsize, data, dataSize);
    return 1;
}
