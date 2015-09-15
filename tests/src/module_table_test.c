#include "../src/globals.c"
#include "../helpers/strings.c"
#include "../src/type_info.c"
#include "../src/value.c"

#include "../src/symbol_table.c"
#include "../src/type_table.c"
#include "../src/ast.c"

#include "../src/module_table.c"

#include "c_test.h"
#include "test_helpers.h"

static struct Ast *program;
static struct ModuleTable *imports;
static struct Module *module;

void alloc_things(void) {
    program = calloc(sizeof *program, 1);
    imports = calloc(sizeof *imports, 1);
    module = calloc(sizeof *module, 1);
}
void free_things(void) {
    free(program);
    free(imports);
    free(module);
}

TEST(ModuleMakeFree) {
    alloc_things();
    assert_eq(R_OK, ModuleMake(module, program, imports), "ModuleMake failed.");
    assert_ne(NULL, module->ModuleScope, "module->ModuleScope not correctly created.");
    assert_ne(NULL, module->TypeTable, "module->TypeTable not correctly created.");
    assert_eq(module->ModuleScope, module->CurrentScope, "module->CurrentScope not set to module->ModuleScope.");
    assert_eq(module->Imports, imports, "module->Imports not set to imports.");
    assert_eq(module->Program, program, "module->Program not set to program.");
    
    assert_eq(R_InvalidArgument, ModuleMake(NULL, program, imports), "ModuleMake should have failed.");

    assert_eq(R_OK, ModuleFree(module), "ModuleFree failed.");
    assert_eq(NULL, module->ModuleScope, "module->ModuleScope not set to NULL.");
    assert_eq(NULL, module->TypeTable, "module->TypeTable not set to NULL.");
    assert_eq(NULL, module->CurrentScope, "module->CurrentScope not set to NULL.");
    assert_eq(NULL, module->Imports, "module->Imports not set NULL.");
    assert_eq(NULL, module->Program, "module->Program not set NULL.");

    assert_eq(R_InvalidArgument, ModuleFree(NULL), "ModuleFree should have failed.");
    free_things();
}

TEST(ModuleTableMakeFree) {
    struct ModuleTable *table = calloc(sizeof *table, 1);
    assert_eq(R_OK, ModuleTableMake(table), "ModuleTabkeMake failed.");
    assert_p(table->NumNodes > 0, "moduleTable->NumNodes not > 0");
    assert_ne(NULL, table->Nodes, "ModuleTable->Nodes == NULL");

    assert_eq(R_InvalidArgument, ModuleTableMake(NULL), "ModuleTableMake should have failed.");

    assert_eq(R_OK, ModuleTableFree(table), "ModuleTableFree failed.");

    assert_eq(R_InvalidArgument, ModuleTableFree(NULL), "ModuleTableFree should have failed.");
    free(table);
}

TEST(ModuleTableInsert) {
    struct ModuleTable *table = calloc(sizeof *table, 1);
    alloc_things();
    ModuleTableMake(table);
    assert_eq(R_OK, ModuleTableInsert(table, "key", module), "ModuleTableInsert failed.");

    assert_eq(R_KeyAlreadyInTable, ModuleTableInsert(table, "key", module), "ModuleTableInsert should have failed.");

    assert_eq(R_InvalidArgument, ModuleTableInsert(NULL, "afhjasga", module), "ModuleTableInsert should have failed.");
    assert_eq(R_InvalidArgument, ModuleTableInsert(table, NULL, module), "ModuleTableInsert should have failed.");
    assert_eq(R_InvalidArgument, ModuleTableInsert(table, "jfkldsh", NULL), "ModuleTableInsert should have failed.");

    ModuleTableFree(table);
    free_things();
    free(table);
}

TEST(ModuleTableFind) {
    struct ModuleTable *table = calloc(sizeof *table, 1);
    struct Module *result;
    char *key;
    int i, gen = 10000;
    alloc_things();
    ModuleTableMake(table);
    assert_eq(0, ModuleTableFind(table, "key", &result), "ModuleTableFind found nonexistent module.");
    assert_eq(NULL, result, "ModuleTableFind did not correctly set out_module");

    ModuleTableInsert(table, "key", module);
    assert_ne(0, ModuleTableFind(table, "key", &result), "ModuleTableFind failed.");
    assert_eq(module, result, "ModuleTableFind failed to set out_module.");

    for (i = 0; i < gen; ++i) {
        key = ident_generator(i + 1);
        assert_ne(R_KeyAlreadyInTable, ModuleTableInsert(table, key, module), "generated duplicate key");
        free(key);
    }
    for (i = 0; i < gen; ++i) {
        key = ident_generator(i + 1);
        assert_ne(0, ModuleTableFind(table, key, &result), "ModuleTableFind failed"); 
        assert_eq(module, result, "ModuleTableFind failed to set out_module");
        free(key);
    }

    ModuleTableFree(table);
    free_things();
    free(table);
}

int main() {
    TEST_RUN(ModuleMakeFree);
    TEST_RUN(ModuleTableMakeFree);
    TEST_RUN(ModuleTableInsert);
    TEST_RUN(ModuleTableFind);
    
    return 0;
}
