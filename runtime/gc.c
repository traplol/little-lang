#include "gc.h"
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
const unsigned int GC_CollectThreshold = 50;

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
    result = ValueFree(object->Value);
#ifndef NDEBUG
    memset(object->Value, 0xff, sizeof *object->Value);
#endif
    free(object->Value);
    GC_Allocated--;
    return result;
}

/************************ Public Functions *************************/

void GC_Disable(void) {
    GC_Disabled = 1;
}

int GC_AllocValue(struct Value **out_value) {
    int result;
    struct GC_Object *object = calloc(sizeof *object, 1);
    struct Value *value = calloc(sizeof *value, 1);
    value->Count = 1;
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
    struct GC_Object *object, *next;
    if (GC_Disabled) {
        return R_OK;
    }
    if (GC_Allocated < GC_CollectThreshold) {
        return R_OK;
    }
    object = GC_Head;
    while (object) {
        next = object->Next;
        if (0 == object->Value->Count) {
            GC_Free(object);
            object->Prev = NULL;
            object->Next = NULL;
            object->Value = NULL;
            free(object);
        }
        object = next;
    }
    return R_OK;
}

#include "runtime/runtime_core.h"
void GC_Dump(void) {
    struct GC_Object *object = GC_Head;
    struct Value *argv[1];
    while (object) {
        argv[0] = object->Value;
        RT_print(1, argv);
        printf(": Count=%d\n", object->Value->Count);
        object = object->Next;
    }
}
