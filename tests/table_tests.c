#include "../src/token.c"
#include "../src/value.c"
#include "../src/table.c"

#include "defines.h"

#include <stdio.h>
#include <assert.h>

#define PASS (printf("%s : PASS\n", __func__))

void test_TableMake_TableFree(void) {
    struct Table typeTable;
    struct Table valueTable;
    assert(0 == TableMake(&typeTable, TableTypeTypeTable, 0) && "Failed to make type table.");
    assert(DEFAULT_TABLE_SIZE == typeTable.TableLength && "Failed to apply default table size.");
    assert(TableTypeTypeTable == typeTable.TableType && "Failed to apply table type.");
    assert(0 == TableFree(&typeTable) && "Failed to free table resources.");
     
    assert(0 == TableMake(&valueTable, TableTypeValueTable, 0) && "Failed to make value table.");
    assert(DEFAULT_TABLE_SIZE == valueTable.TableLength && "Failed to apply default table size.");
    assert(TableTypeValueTable == valueTable.TableType && "Failed to apply table type.");
    assert(0 == TableFree(&valueTable) && "Failed to free table resources.");
    PASS;
}


int main(void) {
    test_TableMake_TableFree();
    return 0;
}
