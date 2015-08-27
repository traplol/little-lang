#include "../src/token.c"
#include "../src/value.c"
#include "../src/table.c"

#include "defines.h"
#include "c_test.h"

char *strdup(const char *s) {
    int l = strlen(s);
    char *r = malloc(l+1);
    r[l] = 0;
    return memcpy(r, s, l);
}

TEST(TableMake_TableFree) {
    struct Table typeTable;
    struct Table valueTable;
    assert_eq(0, TableMake(&typeTable, TableTypeTypeTable, 0), "Failed to make type table.");
    assert_eq(DEFAULT_TABLE_SIZE, typeTable.TableLength, "Failed to apply default table size.");
    assert_eq(TableTypeTypeTable, typeTable.TableType, "Failed to apply table type.");
    assert_eq(0, TableFree(&typeTable), "Failed to free table resources.");

    assert_eq(0, TableMake(&valueTable, TableTypeValueTable, 0), "Failed to make value table.");
    assert_eq(DEFAULT_TABLE_SIZE, valueTable.TableLength, "Failed to apply default table size.");
    assert_eq(TableTypeValueTable, valueTable.TableType, "Failed to apply table type.");
    assert_eq(0, TableFree(&valueTable), "Failed to free table resources.");
}

TEST(TableInsert_TableRemove_TableContains) {
    struct Table table;
    struct TypeInfo *typeInfo, *derivedFrom;
    struct TableEntry *out = NULL;
    char *typeName, *string, *filename;

    TableMake(&table, TableTypeTypeTable, 0);

    typeInfo = calloc(sizeof *typeInfo, 1);
    derivedFrom = calloc(sizeof *derivedFrom, 1);
    typeName = strdup("BaseObject");
    string = strdup("BaseObjectType");
    filename = strdup("TestFilename");
    TypeInfoMake(typeInfo, TypeBaseObject, derivedFrom, typeName, sizeof(void*));

    assert_eq(0, TableInsert(&table, typeInfo, string, filename, 10, 4), "Failed to insert item into table.");
    assert_ne(0, TableContains(&table, typeInfo), "Failed to find item in table.");
    assert_ne(0, TableGetEntry(&table, typeInfo, &out), "Failed to get entry from table");
    assert_ne(NULL, out, "Failed to assign out from TableGetEntry");
    assert_eq(0, TableRemove(&table, &typeInfo), "Failed to remove item from table");
    assert_eq(0, TableContains(&table, &typeInfo), "Table still contains item after removal.");

    TableFree(&table);
}

int main(void) {
    TEST_RUN(TableMake_TableFree);
    TEST_RUN(TableInsert_TableRemove_TableContains);
    return 0;
}
