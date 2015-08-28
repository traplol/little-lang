#include "../src/type_info.c"
#include "../src/value.c"
#include "../src/symbol_table.c"

#include "string.h"
#include "c_test.h"

char *strdup(const char *s) {
    int len = strlen(s);
    char *r = malloc(len + 1);
    return memcpy(r, s, len + 1);
}

TEST(SymbolTableMakeGlobalScope) {
    struct SymbolTable *st = malloc(sizeof *st);
    assert_eq(0, SymbolTableMakeGlobalScope(st), "SymbolTableMakeGlobalScope failed.");
    assert_ne(NULL, st->Symbols, "Failed to allocate table symbols.");
    assert_eq(NULL, st->Parent, "Global scope shouldn't have a parent.");
    assert_gt(st->TableLength, 0, "Global scope length should be greater than zero.");
    SymbolTableFree(st);
}

TEST(SymbolTableFree) {
    struct SymbolTable *st = malloc(sizeof *st);
    SymbolTableMakeGlobalScope(st);
    assert_eq(0, SymbolTableFree(st), "SymbolTableFree failed");
    SymbolTableFree(st);
}

TEST(SymbolTablePushPopScope) {
    struct SymbolTable *st = malloc(sizeof *st);
    struct SymbolTable *global = st;
    SymbolTableMakeGlobalScope(st);
    assert_eq(0, SymbolTablePushScope(&st), "SymbolTablePushScope failed.");
    assert_ne(global, st, "Push scope failed to correctly set current scope.");
    assert_eq(global, st->Parent, "Push scope failed to correctly set parent.");
    assert_eq(0, SymbolTablePopScope(&st), "SymbolTablePopScope failed.");
    assert_eq(global, st, "Pop scope failed to set current scope to the parent.");
    SymbolTableFree(st);
}

TEST(SymbolTableInsert) {
    struct SymbolTable *st = malloc(sizeof *st);
    struct TypeInfo *ti = malloc(sizeof *ti);
    struct Value *v = malloc(sizeof *v);
    int data = 42;
    char *typeName = strdup("Integer");
    char *name = strdup("data");
    char *filename = strdup("testfile");

    
    TypeInfoMake(ti, TypeInteger, ti, typeName, sizeof(int));
    ValueMake(v, ti, &data, sizeof(data));
    SymbolTableMakeGlobalScope(st);
    assert_eq(0, SymbolTableInsert(st, v, name, filename, 1, 5), "Failed to insert symbol into symbol table.");
    SymbolTableFree(st);
}

int main() {
    TEST_RUN(SymbolTableMakeGlobalScope);
    TEST_RUN(SymbolTableFree);
    TEST_RUN(SymbolTablePushPopScope);
    TEST_RUN(SymbolTableInsert);
    
    return 0;
}
