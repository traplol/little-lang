#include "llvector.h"
#include "globals.h"

#include "result.h"

#include <stdlib.h>

static inline unsigned int min(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}

int LLVectorMake(struct LLVector *vector, unsigned int capacity) {
    unsigned int i;
    if (!vector || 0 == capacity) {
        return R_InvalidArgument;
    }
    vector->Index = 0;
    vector->Capacity = capacity;
    vector->Length = capacity;
    vector->Values = calloc(sizeof *vector->Values, vector->Capacity);
    for (i = 0; i < capacity; ++i) {
        vector->Values[i] = &g_TheNilValue;
    }
    return R_OK;
}
int LLVectorFree(struct LLVector *vector) {
    if (!vector) {
        return R_InvalidArgument;
    }
    vector->Index = 0;
    vector->Length = 0;
    vector->Capacity = 0;
    free(vector->Values);
    return R_OK;
}
int LLVectorResize(struct LLVector *vector, unsigned int newSize) {
    struct Value **newValues;
    unsigned int i, limit;
    if (!vector || 0 == newSize) {
        return R_InvalidArgument;
    }
    if (newSize < vector->Capacity && newSize > vector->Length) {
        vector->Capacity = newSize;
        return R_OK;
    }
    newValues = calloc(sizeof *vector->Values, newSize);
    limit = min(vector->Length, newSize);
    for (i = 0; i < limit; ++i) {
        newValues[i] = vector->Values[i];
    }
    vector->Length = limit;
    vector->Capacity = newSize;
    vector->Values = newValues;
    return R_OK;
}
int LLVectorAppendValue(struct LLVector *vector, struct Value *value) {
    if (!vector) {
        return R_InvalidArgument;
    }
    if (vector->Length + 1 >= vector->Capacity) {
        LLVectorResize(vector, vector->Capacity * 2);
    }
    if (vector->Index == vector->Length) {
        vector->Length++;
    }
    vector->Values[vector->Index++] = value;
    return R_OK;
}
int LLVectorSlice(struct LLVector *vector, unsigned int s, unsigned int e, struct LLVector **out_vector) {
    struct LLVector *slice;
    if (!vector || !out_vector) {
        return R_InvalidArgument;
    }
    /* TODO: These are separated so they can have/generate a unique error */
    if (e < s) {
        return R_InvalidArgument;
    }
    if (s >= vector->Length) {
        return R_InvalidArgument;
    }
    if (e >= vector->Length) {
        return R_InvalidArgument;
    }
    slice = calloc(sizeof *slice, 1);
    LLVectorMake(slice, e-s+1);
    while (s < e) {
        LLVectorAppendValue(slice, vector->Values[s]);
        s++;
    }
    *out_vector = slice;
    return R_OK;
}
