#ifndef _LITTLE_LANG_RUNETIME_GC_H
#define _LITTLE_LANG_RUNETIME_GC_H

#include "value.h"
#include "symbol_table.h"

int GC_Collect(void);
int GC_AllocValue(struct Value **out_value);
unsigned int GC_isDisabled(void);
void GC_Disable(void);
void GC_Enable(void);
void GC_Dump(void);
void GC_DumpReachable(void);
int GC_RegisterSymbolTable(struct SymbolTable *st);

#endif
