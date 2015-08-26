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
    struct TypeInfo *DerivedFrom; /* All objects are derived from at least the BaseObject */
    char *TypeName;
    unsigned int Size;
    struct Member {
        char *Name;
        struct TypeInfo *TypeInfo;
        unsigned int MemOffset;
    } **Members;
    unsigned int MembersLen;
    unsigned int CurrentMemberIdx;
};

struct Value {
    struct TypeInfo *TypeInfo;
    union {
        int Boolean;
        int Integer;
        double Real;
        void *UserObject;
        unsigned char __ptrsize[sizeof(void*)]; /* only set data with this. */
    } v;
};

/* Initializes the value. */
int ValueMake(struct Value *value, struct TypeInfo *typeInfo, void *data, unsigned int dataSize);
/* Frees the value's data. */
int ValueFree(struct Value *value);

/* Initializes the type info. */
int TypeInfoMake(struct TypeInfo *typeInfo, enum TypeInfoType type, struct TypeInfo *derivedFrom, char *typeName, unsigned int size);
/* Frees the type info's data. */
int TypeInfoFree(struct TypeInfo *typeInfo);
/* Inserts a new member into the type info. */
int TypeInfoInsertMember(struct TypeInfo *typeInfo, char *name, struct TypeInfo *memberTypeInfo, unsigned int memOffset);
/* Lookup a member, returns a non-zero integer if the member was found and sets
 * out_member to the found member. */
int TypeInfoLookupMember(struct TypeInfo *typeInfo, char *name, struct Member *out_member);
/* Returns a non-zero integer if the type contains the member matching the name. */
int TypeInfoHasMember(struct TypeInfo *typeInfo, char *name);

#endif
