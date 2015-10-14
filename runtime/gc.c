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
#ifdef NO_GC
static int GC_Disabled = 1;
#else
static int GC_Disabled;
#endif
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

void GC_PrintObject(struct GC_Object *object) {
    char *s;
    struct Value *v = object->Value;
    printf("<%08zx>", (size_t)v);
    if (v->IsSymbol) {
        printf("%s -> <%08zx>", v->v.Symbol->Key, (size_t)v->v.Symbol->Value);
        v = v->v.Symbol->Value;
    }
    s = ValueToString(v);
    printf("%s(%s) : Visited=%d\n", v->TypeInfo->TypeName, s, object->Value->Visited);
    free(s);
}

void GC_Dump(void) {
    struct GC_Object *object = GC_Head;
    while (object) {
        GC_PrintObject(object);
        object = object->Next;
    }
}

void GC_MarkObject(struct Value *v);
void GC_MarkSymbolTable(struct SymbolTable *st);
void GC_MarkObjectMembers(struct Value *o) {
    GC_MarkSymbolTable(o->Members);
}

void GC_MarkVector(struct LLVector *v) {
    unsigned int i;
    for (i = 0; i < v->Length; ++i) {
        GC_MarkObject(v->Values[i]);
    }
}

void GC_MarkObject(struct Value *v) {
    v->Visited = 1;
    if (TypeUserObject == v->TypeInfo->Type) {
        GC_MarkObjectMembers(v);
    }
    else if (&g_TheVectorTypeInfo == v->TypeInfo) {
        GC_MarkVector(v->v.Vector);
    }
}

void GC_MarkSymbols(struct Symbol *s) {
    while (s) {
        s->Value->Visited = 1;
        GC_MarkObject(s->Value);
        s = s->Next;
    }
}
void GC_MarkSymbolTable(struct SymbolTable *st) {
    unsigned int i;
    while (st) {
        for (i = 0; i < st->TableLength; ++i) {
            if (st->Symbols[i]) {
                GC_MarkSymbols(st->Symbols[i]);
            }
        }
        st = st->Child;
    }
}
void GC_MarkScopeHolders(struct ScopeHolder *sh) {
    while (sh) {
        GC_MarkSymbolTable(sh->ST);
        sh = sh->Next;
    }
}
void GC_Mark(void) {
    unsigned int i;
    GC_MarkSymbolTable(&g_TheUberScope);
    for (i = 0; i < ScopesSize; ++i) {
        if (Scopes[i]) {
            GC_MarkScopeHolders(Scopes[i]);
        }
    }
}

void GC_Sweep(void) {
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

struct ScopeHolder *ScopeHolderMake(struct SymbolTable *st) {
    struct ScopeHolder *sh = calloc(sizeof *sh, 1);
    sh->ST = st;
    return sh;
}

/************************ Public Functions *************************/

void GC_Disable(void) {
    GC_Disabled = 1;
}

int GC_RegisterSymbolTable(struct SymbolTable *st) {
    unsigned int idx = (size_t)st % ScopesSize;
    struct ScopeHolder *sh = Scopes[idx];
    if (!sh) {
        sh = ScopeHolderMake(st);
        Scopes[idx] = sh;
        return R_OK;;
    }
    while (sh->Next) {
        if (st == sh->ST) {
            return R_KeyAlreadyInTable;
        }
        sh = sh->Next;
    }
    sh->Next = ScopeHolderMake(st);
    return R_OK;
}

int GC_AllocValue(struct Value **out_value) {
    int result;
    struct GC_Object *object = calloc(sizeof *object, 1);
    struct Value *value = calloc(sizeof *value, 1);
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

int GC_Collect(void) {
    if (GC_Disabled) {
        return R_OK;
    }
    if (GC_Allocated < GC_CollectThreshold) {
        return R_OK;
    }
//    printf("%d -> ", GC_Allocated);
    GC_Mark();
    GC_Sweep();
//    printf("%d\n", GC_Allocated);
    return R_OK;
}

