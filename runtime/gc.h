#ifndef _LITTLE_LANG_RUNETIME_GC_H
#define _LITTLE_LANG_RUNETIME_GC_H

#include "value.h"

int GC_Collect(void);
int GC_AllocValue(struct Value **out_value);
void GC_Disable(void);

#endif
