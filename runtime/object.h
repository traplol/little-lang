#ifndef _LITTLE_LANG_RUNTIME_OBJECT_H
#define _LITTLE_LANG_RUNTIME_OBJECT_H

#include "value.h"

extern BuiltinFnProc_t RT_Object_NewAllocator;

int RT_Object_RegisterBuiltins(void);

#endif
