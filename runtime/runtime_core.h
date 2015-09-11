#ifndef _LITTLE_LANG_RUNTIME_RUNTIME_CORE_H
#define _LITTLE_LANG_RUNTIME_RUNTIME_CORE_H

#include "value.h"
#include "little_lang_machine.h"

extern BuiltinFnProc_t RT_string;
extern BuiltinFnProc_t RT_print;

int RegisterRuntime_core(struct LittleLangMachine *llm);

#endif
