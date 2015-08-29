#include "../src/type_info.c"
#include "../src/type_table.c"

#include "c_test.h"
#include "test_helpers.h"


TEST(TypeTableMakeFree) {
    struct TypeTable *tt = malloc(sizeof *tt);
    assert_eq(0, TypeTableMake(tt, 0), "TypeTableMake failed");
    assert_gt(tt->TableLength, 0, "Failed to set default table length.");
    assert_eq(0, TypeTableFree(tt), "TypeTableFree failed");
    free(tt);
}
TEST(TypeTableInsert) {
    struct TypeTable *tt = malloc(sizeof *tt);
    struct TypeInfo *baseObject = malloc(sizeof *baseObject);
    char *baseObjectName = strdup("BaseObject");
    TypeInfoMake(baseObject, TypeBaseObject, NULL, baseObjectName);
    TypeTableMake(tt, 0);
    assert_eq(0, TypeTableInsert(tt, baseObject), "Failed to insert into type table");
    assert_ne(0, TypeTableInsert(tt, baseObject), "Failed to skip insert of duplicate type.");
    TypeTableFree(tt);
    free(baseObject);
    free(tt);
}
TEST(TypeTableFind) {
    struct TypeTable *tt = malloc(sizeof *tt);
    struct TypeInfo *baseObject = malloc(sizeof *baseObject);
    struct TypeInfo *out = NULL;
    char *baseObjectName = strdup("BaseObject");
    TypeInfoMake(baseObject, TypeBaseObject, NULL, baseObjectName);
    TypeTableMake(tt, 0);
    assert_eq(0, TypeTableFind(tt, "BaseObject", &out), "TypeTableFind found something when it shouln't");
    TypeTableInsert(tt, baseObject);
    assert_ne(0, TypeTableFind(tt, "BaseObject", &out), "TypeTableFind failed to find something it should.");
    assert_ne(NULL, out, "TypeTableFind did not correctly assign out.");
    TypeTableFree(tt);
    free(baseObject);
    free(tt);
}

int main() {
    TEST_RUN(TypeTableMakeFree);
    TEST_RUN(TypeTableInsert);
    TEST_RUN(TypeTableFind);
    return 0;
}
