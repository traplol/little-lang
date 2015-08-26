#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

enum TypeInfoType {
    TypeBaseObject,           /* The base object every object is derived from. */
    TypeBoolean,              /* true or false object. */
    TypeInteger,              /* Integer object */
    TypeReal,                 /* Floating point object*/
    TypeUserObject,           /* A user defined object */

    TypeFunction,             /* Functions are first class objects */
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
        int Boolean;
        int Integer;
        double Real;
        void *UserObject;
        unsigned char __ptrsize[sizeof(void*)]; /* only set data with this. */
    } v;
};

/* Initializes the value. */
int ValueMake(struct Value *value, struct TypeInfo typeInfo, void *data, unsigned int dataSize);
/* Frees the value's data. */
int ValueFree(struct Value *value);

#endif
