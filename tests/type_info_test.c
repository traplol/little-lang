#include "../src/type_info.c"

#include "c_test.h"
#include "test_helpers.h"

TEST(TypeInfoMakeFree) {
    struct TypeInfo *ti = malloc(sizeof *ti);
    char *typeName = strdup("BaseObject");
    assert_eq(0, TypeInfoMake(ti, TypeBaseObject, NULL, typeName, sizeof(void*)), "TypeInfoMake failed");
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
    TypeInfoMake(baseObject, TypeBaseObject, NULL, baseObjName, sizeof(void*));
    TypeInfoMake(intObject, TypeInteger, baseObject, intObjName, sizeof(void*));
    TypeInfoMake(userObject, TypeUserObject, baseObject, userObjName, sizeof(void*));
    assert_eq(0, TypeInfoInsertMember(userObject, myMember, intObject, 0), "TypeInfoInsertMember failed");

    TypeInfoFree(userObject);
    TypeInfoFree(intObject);
    TypeInfoFree(baseObject);
    free(userObject);
    free(intObject);
    free(baseObject);
}

TEST(TypeInfoLookupMember) {
    struct Member *out;
    struct TypeInfo *baseObject = malloc(sizeof *baseObject);
    struct TypeInfo *intObject = malloc(sizeof *intObject);
    struct TypeInfo *userObject = malloc(sizeof *userObject);
    char *baseObjName = strdup("BaseObject");
    char *intObjName = strdup("Integer");
    char *userObjName = strdup("MyObject");
    char *myMember = strdup("MyMember");
    TypeInfoMake(baseObject, TypeBaseObject, NULL, baseObjName, sizeof(void*));
    TypeInfoMake(intObject, TypeInteger, baseObject, intObjName, sizeof(void*));
    TypeInfoMake(userObject, TypeUserObject, baseObject, userObjName, sizeof(void*));
    TypeInfoInsertMember(userObject, myMember, intObject, 0);
    
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
