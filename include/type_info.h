#ifndef _LITTLE_LANG_TYPE_INFO_H
#define _LITTLE_LANG_TYPE_INFO_H

enum TypeInfoType {
    TypeBaseObject,           /* The base object every object is derived from. */
    TypeBoolean,              /* true or false object */
    TypeString,               /* A string object */
    TypeInteger,              /* Integer object */
    TypeReal,                 /* Floating point object*/
    TypeUserObject,           /* A user defined object */

    TypeFunction,             /* Functions are first class objects */
};

/* Every type implements at least the default version of the following methods:
 *
 * Arithmetic/math
 * __add__(self, other)         alias => self + other
 * __sub__(self, other)         alias => self - other
 * __mul__(self, other)         alias => self * other
 * __div__(self, other)         alias => self / other
 * __mod__(self, other)         alias => self % other
 * __and__(self, other)         alias => self & other
 * __or__(self, other)          alias => self | other
 * __xor__(self, other)         alias => self ^ other
 * __pow__(self, other)         alias => self ** other
 * __neg__(self)                alias => - self
 * __pos__(self)                alias => + self
 * __lshift__(self, other)      alias => self << other
 * __rshift__(self, other)      alias => self >> other
 *
 * Comparison/logic
 * __not__(self)                alias => ! self
 * __eq__(self, other)          alias => self == other
 * __lt__(self, other)          alias => self < other
 * __gt__(self, other)          alias => self > other
 *
 * Note: __eq__ is expected to return a true or false value
 * Note: __lt__ is expected to return a true or false value
 * Note: __eq__ is expected to return a true or false value
 *
 * Note: != returns the opposite of __eq__
 * Note: <= returns __lt__ OR __eq__
 * Note: >= returns __gt__ OR __eq__
 *
 * Misc.
 * __index__(self, other)       alias => self[other]
 * __str__(self)                called by => str(thing)
 * __hash__(self)               called by => hash(thing)
 * __dbg__(self)                called by => dbg(thing)
 *
 * Note: __str__ is expected to return a String object
 * Note: __hash__ is expected to return an Integer object
 * Note: __dbg__ is expected to return a String object
 */

struct TypeInfo {
    enum TypeInfoType Type;
    struct TypeInfo *DerivedFrom; /* All objects are derived from at least the BaseObject including the BaseObject */
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

/* Initializes the type info. */
int TypeInfoMake(struct TypeInfo *typeInfo, enum TypeInfoType type, struct TypeInfo *derivedFrom, char *typeName);
/* Frees the type info's data. */
int TypeInfoFree(struct TypeInfo *typeInfo);
/* Inserts a new member into the type info. */
int TypeInfoInsertMember(struct TypeInfo *typeInfo, char *name, struct TypeInfo *memberTypeInfo);
/* Lookup a member, returns a non-zero integer if the member was found and sets
 * out_member to the found member. */
int TypeInfoLookupMember(struct TypeInfo *typeInfo, char *name, struct Member **out_member);

#endif
