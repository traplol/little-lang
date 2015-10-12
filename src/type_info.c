#include "type_info.h"
#include "value.h"
#include "helpers/strings.h"
#include "globals.h"
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


/******************* Public Functions *******************/

int TypeInfoMake(struct TypeInfo *typeInfo, enum TypeInfoType type, struct TypeInfo *derivedFrom, char *typeName) {
    struct SymbolTable *methodTable;
    if (!typeInfo || !typeName || typeInfo == derivedFrom) {
        return R_InvalidArgument;
    }
    typeInfo->Type = type;
    typeInfo->DerivedFrom = derivedFrom;
    typeInfo->TypeName = strdup(typeName);
    typeInfo->Members = calloc(sizeof(*typeInfo->Members), MEMBERS_BASE_LENGTH);
    if (!typeInfo->Members) {
        return R_AllocFailed;
    }
    methodTable = calloc(sizeof *methodTable, 1);
    SymbolTableMake(methodTable);
    typeInfo->MethodTable = methodTable;
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
        AstFree(typeInfo->Members[i]);
        free(typeInfo->Members[i]);
    }
    free(typeInfo->Members);
    free(typeInfo->TypeName);
    return R_OK;
}

int TypeInfoInsertMethod(struct TypeInfo *typeInfo, struct Value *method, struct SrcLoc srcLoc) {
    char *name;
    if (!typeInfo || !method) {
        return R_InvalidArgument;
    }
    if (TypeFunction != method->TypeInfo->Type) {
        return R_InvalidArgument;
    }
    if (method->IsBuiltInFn) {
        name = method->v.BuiltinFn->Name;
    }
    else {
        name = method->v.Function->Name;
    }
    return SymbolTableAssign(typeInfo->MethodTable, method, name, 0, srcLoc);
}
int TypeInfoInsertMember(struct TypeInfo *typeInfo, struct Ast *ast) {
    int result;
    if (TypeInfoIsInvalid(typeInfo) || !ast) {
        return R_InvalidArgument;
    }
    if (typeInfo->CurrentMemberIdx + 1 >= typeInfo->MembersLen) {
        result = TypeInfoResizeMembers(typeInfo);
        if (R_OK != result) {
            return result;
        }
    }
    typeInfo->Members[typeInfo->CurrentMemberIdx++] = ast;
    return R_OK;
}

int TypeInfoLookupMethod(struct TypeInfo *typeInfo, char *methodName, struct Value **out_method) {
    struct Symbol *out;
    if (!typeInfo || !methodName || !out_method) {
        return R_InvalidArgument;
    }
    while (1) {
        SymbolTableFindLocal(typeInfo->MethodTable, methodName, &out);
        if (out) {
            *out_method = out->Value;
            return R_OK;
        }
        if (&g_TheBaseObjectTypeInfo == typeInfo) {
            break;
        }
        typeInfo = typeInfo->DerivedFrom;
    }
    *out_method = NULL;
    return R_MethodNotFound;
}
