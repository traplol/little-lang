#include "../src/type_info.c"
#include "../src/value.c"
#include "../src/symbol_table.c"

#include "string.h"
#include "c_test.h"
#include "test_helpers.h"


TEST(SymbolTableMakeGlobalScope) {
    struct SymbolTable *st = malloc(sizeof *st);
    assert_eq(0, SymbolTableMakeGlobalScope(st), "SymbolTableMakeGlobalScope failed.");
    assert_ne(NULL, st->Symbols, "Failed to allocate table symbols.");
    assert_eq(NULL, st->Parent, "Global scope shouldn't have a parent.");
    assert_gt(st->TableLength, 0, "Global scope length should be greater than zero.");
    SymbolTableFree(st);
    free(st);
}

TEST(SymbolTableFree) {
    struct SymbolTable *st = malloc(sizeof *st);
    SymbolTableMakeGlobalScope(st);
    assert_eq(0, SymbolTableFree(st), "SymbolTableFree failed");
    free(st);
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
    free(st);
}

TEST(SymbolTableInsert) {
    struct SymbolTable *st = malloc(sizeof *st);
    struct TypeInfo *ti = malloc(sizeof *ti);
    struct Value *v = malloc(sizeof *v);
    int data = 42;
    char *typeName = strdup("Integer");
    char *name = strdup("data");
    char *filename = strdup("testfile");

    
    TypeInfoMake(ti, TypeInteger, NULL, typeName, sizeof(int));
    ValueMake(v, ti, &data, sizeof(data));
    SymbolTableMakeGlobalScope(st);
    assert_eq(0, SymbolTableInsert(st, v, name, filename, 1, 5), "Failed to insert symbol into symbol table.");
    SymbolTableFree(st);

    TypeInfoFree(ti);
    free(ti);
    free(filename);
    free(v);
    free(st);
}

TEST(SymbolTableFindLocal) {
    struct SymbolTable *st = malloc(sizeof *st);
    struct TypeInfo *ti = malloc(sizeof *ti);
    struct Value *v = malloc(sizeof *v);
    struct Symbol *out;
    int data = 42;
    char *typeName = strdup("Integer");
    char *name = strdup("data");
    char *filename = strdup("testfile");

    
    TypeInfoMake(ti, TypeInteger, NULL, typeName, sizeof(int));
    ValueMake(v, ti, &data, sizeof(data));
    SymbolTableMakeGlobalScope(st);
    SymbolTableInsert(st, v, name, filename, 1, 5);

    assert_ne(0, SymbolTableFindLocal(st, name, &out), "Failed to find key local scope.");
    assert_eq(v, out->Value, "SymbolTableFindNearest did not set out variable correctly.");
    
    SymbolTableFree(st);
    TypeInfoFree(ti);
    free(ti);
    free(filename);
    free(v);
    free(st);
}

TEST(SymbolTableFindNearest) {
    struct SymbolTable *st = malloc(sizeof *st);
    struct TypeInfo *ti = malloc(sizeof *ti);
    struct Value *v = malloc(sizeof *v);
    struct Symbol *out;
    int data = 42;
    char *typeName = strdup("Integer");
    char *name = strdup("data");
    char *filename = strdup("testfile");

    
    TypeInfoMake(ti, TypeInteger, NULL, typeName, sizeof(int));
    ValueMake(v, ti, &data, sizeof(data));
    SymbolTableMakeGlobalScope(st);
    SymbolTableInsert(st, v, name, filename, 1, 5);
    SymbolTablePushScope(&st);

    assert_ne(0, SymbolTableFindNearest(st, name, &out), "Failed to find key local scope.");
    assert_eq(v, out->Value, "SymbolTableFindNearest did not set out variable correctly.");
    SymbolTablePopScope(&st);
    
    SymbolTableFree(st);
    TypeInfoFree(ti);
    free(ti);
    free(filename);
    free(v);
    free(st);
}

int main() {
    TEST_RUN(SymbolTableMakeGlobalScope);
    TEST_RUN(SymbolTableFree);
    TEST_RUN(SymbolTablePushPopScope);
    TEST_RUN(SymbolTableInsert);
    TEST_RUN(SymbolTableFindLocal);
    TEST_RUN(SymbolTableFindNearest);
    
    return 0;
}
