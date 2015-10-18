#ifndef _LITTLE_LANG_RUNTIME_RUNTIME_CORE_H
#define _LITTLE_LANG_RUNTIME_RUNTIME_CORE_H

#include "value.h"
#include "little_lang_machine.h"

extern BuiltinFnProc_t RT_string;
extern BuiltinFnProc_t RT_print;
extern BuiltinFnProc_t RT_println;
extern BuiltinFnProc_t RT_type;
extern BuiltinFnProc_t RT_hash;
extern BuiltinFnProc_t RT_dbg;

extern BuiltinFnProc_t RT___gc_dump;
extern BuiltinFnProc_t RT___gc_reachable;
extern BuiltinFnProc_t RT___gc_disable;
extern BuiltinFnProc_t RT___gc_enable;
extern BuiltinFnProc_t RT___gc_is_disabled;

int RegisterRuntime_core(void);

#endif
