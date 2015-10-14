#ifndef _LITTLE_LANG_RUNTIME_STRING_H
#define _LITTLE_LANG_RUNTIME_STRING_H

#include "value.h"

extern BuiltinFnProc_t RT_String_Concat;

int RT_String_RegisterBuiltins(void);

#endif
