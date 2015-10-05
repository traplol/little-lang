#include "gc.h"
#include "globals.h"
#include "result.h"

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
const unsigned int GC_CollectThreshold = 1000;

struct ScopeHolder {
    struct SymbolTable *ST;
    struct ScopeHolder *Next;
};
const unsigned int ScopesSize = 53;
static struct ScopeHolder *Scopes[ScopesSize];

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
    if (v->IsSymbol) {
        printf("%s -> ", v->v.Symbol->Key);
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

void GC_MarkSymbols(struct Symbol *s) {
    while (s) {
        s->Value->Visited = 1;
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
    value->Visited = 0;
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
    printf("%d -> ", GC_Allocated);
    GC_Mark();
    GC_Sweep();
    printf("%d\n", GC_Allocated);
    return R_OK;
}

