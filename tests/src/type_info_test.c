#include "../src/type_info.c"

#include "strings.h"
#include "c_test.h"
#include "test_helpers.h"

TEST(TypeInfoMakeFree) {
    struct TypeInfo *ti = malloc(sizeof *ti);
    char *typeName = strdup("BaseObject");
    assert_eq(0, TypeInfoMake(ti, TypeBaseObject, NULL, typeName), "TypeInfoMake failed");
    assert_eq(0, TypeInfoFree(ti), "TypeInfoFree failed");
    free(ti);
}

TEST(TypeInfoInsertMember) {
    struct TypeInfo *baseObject = malloc(sizeof *baseObject);
    struct TypeInfo *intObject = malloc(sizeof *intObject);
    struct TypeInfo *userObject = malloc(sizeof *userObject);
    char *baseObjName = strdup("BaseObject");
    char *intObjName = strdup("Integer");
    char *userObjName = strdup("MyObject");
    char *myMember = strdup("MyMember");
    unsigned int size;
    TypeInfoMake(baseObject, TypeBaseObject, NULL, baseObjName);
    TypeInfoMake(intObject, TypeInteger, baseObject, intObjName);
    TypeInfoMake(userObject, TypeUserObject, baseObject, userObjName);
    size = userObject->Size;
    assert_eq(0, TypeInfoInsertMember(userObject, myMember, intObject), "TypeInfoInsertMember failed");
    assert_gt(userObject->Size, 0, "TypeInfoInsertMember did not update size.");
    assert_gt(userObject->Size, size, "TypeInfoInsertMember did not update size.");
    assert_eq(size, userObject->Size - intObject->Size, "TypeInfoInsertMember did not update size.");

    TypeInfoFree(userObject);
    TypeInfoFree(intObject);
    TypeInfoFree(baseObject);
    free(userObject);
    free(intObject);
    free(baseObject);
}

TEST(TypeInfoLookupMember) {
    struct TypeInfo *baseObject = malloc(sizeof *baseObject);
    struct TypeInfo *intObject = malloc(sizeof *intObject);
    struct TypeInfo *userObject = malloc(sizeof *userObject);
    char *baseObjName = strdup("BaseObject");
    char *intObjName = strdup("Integer");
    char *userObjName = strdup("MyObject");
    char *myMember = strdup("MyMember");
    struct Member *out;
    TypeInfoMake(baseObject, TypeBaseObject, NULL, baseObjName);
    TypeInfoMake(intObject, TypeInteger, baseObject, intObjName);
    TypeInfoMake(userObject, TypeUserObject, baseObject, userObjName);
    TypeInfoInsertMember(userObject, myMember, intObject);
    
    assert_ne(0, TypeInfoLookupMember(userObject, "MyMember", &out), "TypeInfoLookupMember failed");
    assert_ne(NULL, out, "TypeInfoLookupMember failed to assign out.");

    TypeInfoFree(userObject);
    TypeInfoFree(intObject);
    TypeInfoFree(baseObject);
    free(userObject);
    free(intObject);
    free(baseObject);
}

int main() {
    TEST_RUN(TypeInfoMakeFree);
    TEST_RUN(TypeInfoInsertMember);
    TEST_RUN(TypeInfoLookupMember);
    return 0;
}
