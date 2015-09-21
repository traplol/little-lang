#include "../src/globals.c"
#include "../src/ast.c"
#include "../helpers/strings.c"
#include "../src/symbol_table.c"
#include "../src/type_info.c"
#include "../src/value.c"
#include "../runtime/gc.c"

#include "c_test.h"
#include "test_helpers.h"

/* Note that these tests use the current state of the garbage collector
   and do not need to reallocate resources between tests, however, this
   does mean that the order in which they are executed does matter. */

const unsigned int MaxNumValuesAllocated = 10000;
static unsigned int allocated = 100;
static struct Value **ValuesAllocated;

void setup(void) {
    GlobalsInit();
    ValuesAllocated = calloc(sizeof *ValuesAllocated, MaxNumValuesAllocated);
}

void done(void) {
    struct GC_Object *object = GC_Head;
    while (object) {
        object->Value->Count = 0;
        object = object->Next;
    }
    GC_Collect();
    free(ValuesAllocated);
    GlobalsDenit();
}

TEST(GC_Alloc) {
    unsigned int i;
    for (i = 0; i < allocated; ++i) {
        assert_eq(R_OK, GC_AllocValue(&ValuesAllocated[i]), "GC Failed to alloc values");
        assert_ne(0, ValuesAllocated[i]->Count, "GC_AllocValue Failed to set Value->Count");
        ValuesAllocated[i]->TypeInfo = &g_TheIntegerTypeInfo;
    }
}

TEST(CheckValueOrderIsCorrect) {
    unsigned int i = 0;
    struct GC_Object *object = GC_Head;
    while (object) {
        assert_eq(ValuesAllocated[i++], object->Value, "Value Order is incorrect");
        object = object->Next;
    }
}

TEST(GC_CollectHead) {
    ValuesAllocated[0]->Count = 0;
    assert_eq(R_OK, GC_Collect(), "GC_Collect failed to collect head.");
    assert_eq(ValuesAllocated[1], GC_Head->Value, "GC_Collect failed to move head.");
}

TEST(GC_CollectTail) {
    ValuesAllocated[allocated-1]->Count = 0;
    assert_eq(R_OK, GC_Collect(), "GC_Collect failed to collect head.");
    assert_eq(ValuesAllocated[allocated-2], GC_Tail->Value, "GC_Collect failed to move Tail.");
}

TEST(GC_CollectRandomAreNotInList) {
    struct Value *v11 = ValuesAllocated[11];
    struct Value *v21 = ValuesAllocated[21];
    struct Value *v31 = ValuesAllocated[31];
    struct Value *v41 = ValuesAllocated[41];
    struct Value *v51 = ValuesAllocated[51];
    struct Value *cv;
    struct GC_Object *object;

    v11->Count = 0;
    v21->Count = 0;
    v31->Count = 0;
    v41->Count = 0;
    v51->Count = 0;

    assert_eq(R_OK, GC_Collect(), "GC_Collect failed.");
    object = GC_Head;
    while (object) {
        cv = object->Value;
        if (cv == v11 || cv == v21 || cv == v31 || cv == v41 || cv == v51) {
            fail("GC_Collect failed to remove node from list.");
        }
        object = object->Next;
    }
}

int main() {
    setup();
    TEST_RUN(GC_Alloc);
    TEST_RUN(CheckValueOrderIsCorrect);
    TEST_RUN(GC_CollectHead);
    TEST_RUN(GC_CollectTail);
    TEST_RUN(GC_CollectRandomAreNotInList);
    done();
    return 0;
}
