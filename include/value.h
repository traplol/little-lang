#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

enum TypeInfoType {
    TypeBaseObject,           /* The base object every object is derived from. */
    TypeInteger,              /* Raw integer type */
    TypeReal,                 /* Raw double type */
    TypeUserObject,           /* A user defined object */

    TypeFunction,
};

struct TypeInfo {
    enum TypeInfoType Type;
    struct TypeInfo *DerivedFrom;
    char *TypeName;           /* The name of the value's type, used for member lookup. */
    unsigned int Size;
    struct Member {
        char *Name;
        unsigned int Offset;
        unsigned int Size;
    } **Members;
};

struct Value {
    struct TypeInfo TypeInfo;
    union {
        int Integer;
        double Real;
        unsigned char __ptrsize[sizeof(void*)]; /* Largest value size should be a pointer to data. */
    } v;
};

/* Initializes the value. */
int ValueMake(struct Value *value, struct TypeInfo typeInfo, void *data, unsigned int dataSize);
/* Frees the value's data. */
int ValueFree(struct Value *value);

#endif
