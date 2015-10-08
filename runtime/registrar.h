#ifndef _LITTLE_LANG_RUNTIME_REGISTRAR_H
#define _LITTLE_LANG_RUNTIME_REGISTRAR_H

#include "little_lang_machine.h"
#include "value.h"

int RegisterRuntimes(void);

int FunctionMaker(struct Value **out_value, char *name, unsigned int numArgs, int isVarArgs, BuiltinFnProc_t proc);

#endif

