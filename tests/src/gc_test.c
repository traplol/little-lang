#undef NDEBUG
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

const unsigned int MaxNumValuesAllocated = (GC_CollectThreshold + 100) * 3;
static unsigned int allocated = MaxNumValuesAllocated / 2;
static struct Value **ValuesAllocated;

static struct SymbolTable *CurrentScope;

void setup(void) {
    GlobalsInit();
    CurrentScope = g_TheGlobalScope;
    ValuesAllocated = calloc(sizeof *ValuesAllocated, MaxNumValuesAllocated);
}

void done(void) {
    GlobalsDenit();
    GC_Collect();
    free(ValuesAllocated);
}

TEST(GC_Alloc) {
    unsigned int i;
    char *id;
    struct Value *v;
    for (i = 0; i < allocated; ++i) {
        assert_eq(R_OK, GC_AllocValue(&v), "GC Failed to alloc values");
        assert_eq(0, v->Visited, "GC_AllocValue Failed to set Value->Visited");
        v->TypeInfo = &g_TheIntegerTypeInfo;
        v->v.Integer = i;
        id = ident_generator(i);
        if (i && i % (allocated/5) == 0) {
            SymbolTablePushScope(&CurrentScope);
        }
        SymbolTableInsert(CurrentScope, v, id, 0, (struct SrcLoc){"<gc_test.c>", -1, -1});
        ValuesAllocated[i] = v;
        free(id);
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
    GC_Mark();
    GC_Head->Value->Visited = 0;
    GC_Sweep();
    assert_eq(ValuesAllocated[1], GC_Head->Value, "GC_Collect failed to move head.");
}

TEST(GC_CollectTail) {
    GC_Mark();
    GC_Tail->Value->Visited = 0;
    GC_Sweep();
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
    GC_Mark();
    v11->Visited = 0;
    v21->Visited = 0;
    v31->Visited = 0;
    v41->Visited = 0;
    v51->Visited = 0;
    GC_Sweep();
    object = GC_Head;
    while (object) {
        cv = object->Value;
        if (cv == v11 || cv == v21 || cv == v31 || cv == v41 || cv == v51) {
            fail("GC_Collect failed to remove node from list.");
        }
        object = object->Next;
    }
}

struct Values {
    unsigned int Length;
    struct Value **Values;
};
int check_in_vals(struct Values vals, struct Value *v) {
    unsigned int i;
    for (i = 0; i < vals.Length; ++i) {
        if (vals.Values[i] == v) {
            return 1;
        }
    }
    return 0;
}
struct Values get_values(void) {
    struct Value **vals = calloc(sizeof *vals, MaxNumValuesAllocated);
    struct Values values = {MaxNumValuesAllocated, vals};
    unsigned int i, j = 0;
    struct Symbol *s;
    for (i = 0; i < CurrentScope->TableLength; ++i) {
        s = CurrentScope->Symbols[i];
        for (; s; s = s->Next, ++j) {
            vals[j] = s->Value;
        }
    }
    return values;
}

TEST(CheckCollectAfterScopePop) {
    struct Values values = get_values();
    struct GC_Object *object;
    SymbolTablePopScope(&CurrentScope);
    GC_Collect();
    object = GC_Head;
    while (object) {
        if (check_in_vals(values, object->Value)) {
            fail("GC_Collect didn't last scope.");
        }
        object = object->Next;
    }
}

TEST(CheckAllocatedObjectCountIsCorrect) {
    unsigned int count = 0;
    struct GC_Object *obj = GC_Head;
    assert_ne(0, GC_Allocated, "GC_Allocated should not be 0");
    while (obj) {
        ++count;
        obj = obj->Next;
    }
    assert_eq(count, GC_Allocated, "GC_Allocated != count");
}

TEST(CheckGCForGarbageAfterCollect) {
    struct GC_Object *obj;
    struct TypeInfo *bad;
    memset(&bad, 0xff, sizeof bad);
    GC_Collect();
    obj = GC_Head;
    while (obj) {
        assert_ne(bad, obj->Value->TypeInfo, "Found garbage still in list.");
        obj = obj->Next;
    }
}

int main() {
    setup();
    TEST_RUN(GC_Alloc);
    TEST_RUN(CheckValueOrderIsCorrect);
    TEST_RUN(GC_CollectHead);
    TEST_RUN(GC_CollectTail);
    TEST_RUN(GC_CollectRandomAreNotInList);
    TEST_RUN(CheckCollectAfterScopePop);
    TEST_RUN(CheckGCForGarbageAfterCollect);
    TEST_RUN(CheckAllocatedObjectCountIsCorrect);
    done();
    return 0;
}
