#include "type_info.h"

#include <stdlib.h>
#include <string.h>

#define MEMBERS_BASE_LENGTH 4

/******************* Helpers *******************/

int TypeInfoIsValid(struct TypeInfo *typeInfo) {
    return typeInfo && typeInfo->TypeName && typeInfo->Members;
}

int TypeInfoIsInvalid(struct TypeInfo *typeInfo) {
    return !TypeInfoIsValid(typeInfo);
}

int TypeInfoResizeMembers(struct TypeInfo *typeInfo) {
    if (TypeInfoIsInvalid(typeInfo)) {
        return -1;
    }
    typeInfo->MembersLen *= 1.5;
    typeInfo->Members = realloc(typeInfo->Members, typeInfo->MembersLen * sizeof(*typeInfo->Members));
    if (!typeInfo->Members) {
        return -1;
    }
    return 0;
}

int TypeInfoMemberFree(struct Member *member) {
    if (!member) {
        return -1;
    }
    free(member->Name);
    return 0;
}


/******************* Public Functions *******************/

int TypeInfoMake(struct TypeInfo *typeInfo, enum TypeInfoType type, struct TypeInfo *derivedFrom, char *typeName) {
    unsigned int size;
    if (!typeInfo || !typeName || typeInfo == derivedFrom) {
        return -1;
    }
    typeInfo->Type = type;
    typeInfo->DerivedFrom = derivedFrom;
    typeInfo->TypeName = typeName;
    switch (type) {
        case TypeFunction: 
        case TypeBaseObject: 
        case TypeNil: /* Pointer to the nil object. */
        case TypeBoolean: /* Booleans are pointers that point to the "true" or "false" object */
        case TypeUserObject: size = sizeof(void*); break;
        case TypeInteger: size = sizeof(int); break;
        case TypeReal: size = sizeof(double); break;
    }
    typeInfo->Size = size;
    typeInfo->Members = calloc(sizeof(*typeInfo->Members), MEMBERS_BASE_LENGTH);
    if (!typeInfo->Members) {
        return -1;
    }
    typeInfo->MembersLen = MEMBERS_BASE_LENGTH;
    typeInfo->CurrentMemberIdx = 0;
    return 0;
}
int TypeInfoFree(struct TypeInfo *typeInfo) {
    unsigned int i;
    if (TypeInfoIsInvalid(typeInfo)) {
        return -1;
    }

    for(i = 0; i < typeInfo->MembersLen; ++i) {
        TypeInfoMemberFree(typeInfo->Members[i]);
        free(typeInfo->Members[i]);
    }
    free(typeInfo->Members);
    free(typeInfo->TypeName);
    return 0;
}

int TypeInfoInsertMember(struct TypeInfo *typeInfo, char *name, struct TypeInfo *memberTypeInfo) {
    struct Member *member;
    if (TypeInfoIsInvalid(typeInfo) || TypeInfoIsInvalid(memberTypeInfo) || !name) {
        return -1;
    }
    if (typeInfo->CurrentMemberIdx + 1 >= typeInfo->MembersLen) {
        if (0 != TypeInfoResizeMembers(typeInfo)) {
            return -1;
        }
    }
    member = malloc(sizeof *member);
    member->Name = name;
    member->TypeInfo = memberTypeInfo;
    member->MemOffset = typeInfo->Size;
    typeInfo->Size += memberTypeInfo->Size;
    typeInfo->Members[typeInfo->CurrentMemberIdx++] = member;
    return 0;
}

/* TODO: Implement better member lookup than O(n) */
struct Member *TypeInfoGetMember(struct TypeInfo *typeInfo, char *name) {
    unsigned int i;
    for (i = 0; i < typeInfo->CurrentMemberIdx; ++i) {
        if (0 == strcmp(typeInfo->Members[i]->Name, name)) {
            return typeInfo->Members[i];
        }
    }
    return NULL;
}

int TypeInfoLookupMember(struct TypeInfo *typeInfo, char *name, struct Member **out_member) {
    struct Member *temp;
    if (TypeInfoIsInvalid(typeInfo) || !name) {
        return 0;
    } 
    temp = TypeInfoGetMember(typeInfo, name);
    if (!temp) {
        return 0;
    }
    if (out_member) {
        *out_member = temp;
    }
    return 1;
}
