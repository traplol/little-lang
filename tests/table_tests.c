#include "../src/token.c"
#include "../src/value.c"
#include "../src/table.c"

#include "defines.h"
#include "c_test.h"

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

int main(void) {
    TEST_RUN(TableMake_TableFree);
    return 0;
}
