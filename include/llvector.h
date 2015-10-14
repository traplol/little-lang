#ifndef _LITTLE_LANG_LLVECTOR_H
#define _LITTLE_LANG_LLVECTOR_H

struct LLVector {
    unsigned int Length;
    unsigned int Capacity;
    struct Value **Values;
};

int LLVectorMake(struct LLVector *vector);
int LLVectorResize(struct LLVector *vector, unsigned int newSize);
int LLVectorAppendValue(struct LLVector *vector, struct Value *value);
int LLVectorSlice(struct LLVector *vector, unsigned int s, unsigned int e, struct LLVector **out_vector);

#endif
