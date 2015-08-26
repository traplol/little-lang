#ifndef _LITTLE_LANG_VALUE_H
#define _LITTLE_LANG_VALUE_H

enum Type {
    TypeBaseObject,           /* The base object every object is derived from. */
    TypeInteger,              /* Raw integer type */
    TypeReal,                 /* Raw double type */
    TypeUserObject            /* A user defined object */
};

struct Value {
    enum Type Type;
    char *TypeName;           /* The name of the value's type, used for member lookup. */
    unsigned int SizeOfValue; /* Max should be the size of a pointer. */
    union {
        int Integer;
        double Real;
        unsigned char __ptrsize[sizeof(void*)]; /* Largest value size should be a pointer to data. */
    } v;
};


int ValueMake(struct Value *value, enum Type type, void *data, unsigned int dataSize);

#endif
