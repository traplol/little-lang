#include "type_info.h"
#include "result.h"

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
        return R_InvalidArgument;
    }
    typeInfo->MembersLen *= 1.5;
    typeInfo->Members = realloc(typeInfo->Members, typeInfo->MembersLen * sizeof(*typeInfo->Members));
    if (!typeInfo->Members) {
        return R_AllocFailed;
    }
    return R_OK;
}

int TypeInfoMemberFree(struct Member *member) {
    if (!member) {
        return R_InvalidArgument;
    }
    free(member->Name);
    return R_OK;
}


/******************* Public Functions *******************/

int TypeInfoMake(struct TypeInfo *typeInfo, enum TypeInfoType type, struct TypeInfo *derivedFrom, char *typeName) {
    unsigned int size;
    if (!typeInfo || !typeName || typeInfo == derivedFrom) {
        return R_InvalidArgument;
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
        return R_AllocFailed;
    }
    typeInfo->MembersLen = MEMBERS_BASE_LENGTH;
    typeInfo->CurrentMemberIdx = 0;
    return R_OK;
}
int TypeInfoFree(struct TypeInfo *typeInfo) {
    unsigned int i;
    if (TypeInfoIsInvalid(typeInfo)) {
        return R_InvalidArgument;
    }

    for(i = 0; i < typeInfo->MembersLen; ++i) {
        TypeInfoMemberFree(typeInfo->Members[i]);
        free(typeInfo->Members[i]);
    }
    free(typeInfo->Members);
    free(typeInfo->TypeName);
    return R_OK;
}

int TypeInfoInsertMember(struct TypeInfo *typeInfo, char *name, struct TypeInfo *memberTypeInfo) {
    struct Member *member;
    int result;
    if (TypeInfoIsInvalid(typeInfo) || TypeInfoIsInvalid(memberTypeInfo) || !name) {
        return R_InvalidArgument;
    }
    if (typeInfo->CurrentMemberIdx + 1 >= typeInfo->MembersLen) {
        result = TypeInfoResizeMembers(typeInfo);
        if (R_OK != result) {
            return result;
        }
    }
    member = malloc(sizeof *member);
    member->Name = name;
    member->TypeInfo = memberTypeInfo;
    member->MemOffset = typeInfo->Size;
    typeInfo->Size += memberTypeInfo->Size;
    typeInfo->Members[typeInfo->CurrentMemberIdx++] = member;
    return R_OK;
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
        return R_False;
    } 
    temp = TypeInfoGetMember(typeInfo, name);
    if (!temp) {
        return R_False;
    }
    if (out_member) {
        *out_member = temp;
    }
    return R_True;
}
