#include "gc.h"
#include "globals.h"
#include "result.h"

#include "helpers/macro_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct GC_Object {
    struct GC_Object *Prev, *Next;
    struct Value *Value;
};

static struct GC_Object *GC_Head;
static struct GC_Object *GC_Tail;
static unsigned int GC_Allocated;
static int GC_Disabled;
/* TODO: Proper size of memory allocated rather than number of objects. */
#ifdef GC_COLLECT_THRESHOLD
const unsigned int GC_CollectThreshold = GC_COLLECT_THRESHOLD;
#else
const unsigned int GC_CollectThreshold = 0;
#endif

struct ScopeHolder {
    struct SymbolTable *ST;
    struct ScopeHolder *Next;
};
#define SCOPE_SIZE 53U
const unsigned int ScopesSize = SCOPE_SIZE;
static struct ScopeHolder *Scopes[SCOPE_SIZE];

int GC_Append(struct GC_Object *object) {
    if (!object) {
        return R_InvalidArgument;
    }
    if (!GC_Head) {
        GC_Head = object;
        GC_Tail = GC_Head;
    }
    else {
        GC_Tail->Next = object;
        object->Prev = GC_Tail;
        GC_Tail = object;
    }
    return R_OK;
}

int GC_Free(struct GC_Object *object) {
    struct GC_Object *prev, *next;
    int result;
    if (!object) {
        return R_InvalidArgument;
    }
    prev = object->Prev;
    next = object->Next;
    result = ValueFree(object->Value);
    if (R_OK != result) {
        return result;
    }
    if (prev) {
        prev->Next = next;
    }
    if (next) {
        next->Prev = prev;
    }
    if (GC_Head == object) {
        GC_Head = next;
    }
    if (GC_Tail == object) {
        GC_Tail = prev;
    }
#ifndef NDEBUG
    memset(object->Value, 0xff, sizeof *object->Value);
#endif
    free(object->Value);
    GC_Allocated--;
    return result;
}

static void GC_PrintValue(struct Value *v) {
    char *s;
    struct Value *symbol = v;
    printf("<%08zx>", (size_t)v);
    if (v->IsSymbol) {
        printf("%s -> <%08zx>", v->v.Symbol->Key, (size_t)v->v.Symbol->Value);
        v = v->v.Symbol->Value;
    }
    s = ValueToString(v);
    printf("%s(%s) : Visited=%d\n", v->TypeInfo->TypeName, s, symbol->Visited);
    free(s);
}

static void GC_PrintObject(struct GC_Object *object) {
    GC_PrintValue(object->Value);
}

static void GC_VisitValue(struct Value *v) {
    v->Visited = 1;
}

typedef void (*GC_ApplyProcToValue_t)(struct Value *v);

static void GC_VisitObject(struct Value *v, GC_ApplyProcToValue_t fn);
static void GC_VisitSymbolTable(struct SymbolTable *st, GC_ApplyProcToValue_t fn);

static void GC_VisitObjectMembers(struct Value *o, GC_ApplyProcToValue_t fn) {
    GC_VisitSymbolTable(o->Members, fn);
}

static void GC_VisitVector(struct LLVector *v, GC_ApplyProcToValue_t fn) {
    unsigned int i;
    for (i = 0; i < v->Length; ++i) {
        GC_VisitObject(v->Values[i], fn);
    }
}

static void GC_VisitObject(struct Value *v, GC_ApplyProcToValue_t fn) {
    fn(v);
    if (v->TypeInfo && TypeUserObject == v->TypeInfo->Type) {
        GC_VisitObjectMembers(v, fn);
    }
    else if (&g_TheVectorTypeInfo == v->TypeInfo) {
        GC_VisitVector(v->v.Vector, fn);
    }
}

static void GC_VisitSymbols(struct Symbol *s, GC_ApplyProcToValue_t fn) {
    while (s) {
        s->Value->Visited = 1;
        GC_VisitObject(s->Value, fn);
        s = s->Next;
    }
}
static void GC_VisitSymbolTable(struct SymbolTable *st, GC_ApplyProcToValue_t fn) {
    unsigned int i;
    while (st) {
        for (i = 0; i < st->TableLength; ++i) {
            if (st->Symbols[i]) {
                GC_VisitSymbols(st->Symbols[i], fn);
            }
        }
        st = st->Child;
    }
}
static void GC_VisitScopeHolders(struct ScopeHolder *sh, GC_ApplyProcToValue_t fn) {
    while (sh) {
        GC_VisitSymbolTable(sh->ST, fn);
        sh = sh->Next;
    }
}

static void GC_VisitReachableScopes(GC_ApplyProcToValue_t fn) {
    unsigned int i;
    for (i = 0; i < ScopesSize; ++i) {
        if (Scopes[i]) {
            GC_VisitScopeHolders(Scopes[i], fn);
        }
    }
}

static void GC_VisitTheUberScope(GC_ApplyProcToValue_t fn) {
    GC_VisitSymbolTable(&g_TheUberScope, fn);
}

static void GC_VisitEverything(GC_ApplyProcToValue_t fn) {
    GC_VisitTheUberScope(fn);
    GC_VisitReachableScopes(fn);
}

static void GC_Mark(void) {
    GC_VisitEverything(GC_VisitValue);
}

static void GC_Sweep(void) {
    struct GC_Object *object = GC_Head;
    struct GC_Object *next;
    while (object) {
        next = object->Next;
        if (!object->Value->Visited) {
            GC_Free(object);
            object->Prev = NULL;
            object->Next = NULL;
            object->Value = NULL;
            free(object);
        }
        else {
            object->Value->Visited = 0;
        }
        object = next;
    }
}

static struct ScopeHolder *ScopeHolderAlloc(struct SymbolTable *st) {
    struct ScopeHolder *sh = calloc(sizeof *sh, 1);
    sh->ST = st;
    return sh;
}

/************************ Public Functions *************************/

void GC_Dump(void) {
    struct GC_Object *object = GC_Head;
    while (object) {
        GC_PrintObject(object);
        object = object->Next;
    }
}

void GC_DumpReachable(void) {
    GC_VisitEverything(GC_PrintValue);
}

void GC_Disable(void) {
    GC_Disabled = 1;
}
void GC_Enable(void) {
    GC_Disabled = 0;
}
unsigned int GC_isDisabled(void) {
    return GC_Disabled;
}

int GC_RegisterSymbolTable(struct SymbolTable *st) {
    unsigned int idx = (size_t)st % ScopesSize;
    struct ScopeHolder *sh = Scopes[idx];
    if (!sh) {
        sh = ScopeHolderAlloc(st);
        Scopes[idx] = sh;
        return R_OK;;
    }
    while (sh->Next) {
        if (st == sh->ST) {
            return R_KeyAlreadyInTable;
        }
        sh = sh->Next;
    }
    sh->Next = ScopeHolderAlloc(st);
    return R_OK;
}

int GC_AllocValue(struct Value **out_value) {
    int result;
    struct GC_Object *object;
    struct Value *value;

    /* TODO: Need to fix marking: cycles and symbol lifetime. */
    result = GC_Collect();
    if (R_OK != result) {
        *out_value = NULL;
        return result;
    }

    object = calloc(sizeof *object, 1);
    value = calloc(sizeof *value, 1);
    value->Visited = 1;
    object->Value = value;
    result = GC_Append(object);
    if (R_OK != result) {
        *out_value = NULL;
        free(value);
        free(object);
        return result;
    }
    GC_Allocated++;
    *out_value = value;
    return R_OK;
}

#ifdef NO_GC
int GC_Collect(void) {
    return R_OK;
}
#else
int GC_Collect(void) {
    if (GC_Disabled) {
        return R_OK;
    }
    if (GC_Allocated < GC_CollectThreshold) {
        return R_OK;
    }
    GC_Mark();
    GC_Sweep();
    return R_OK;
}
#endif
