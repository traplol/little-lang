#ifndef _LITTLE_LANG_GLOBALS_H
#define _LITTLE_LANG_GLOBALS_H

extern struct Value g_TheTrueValue;
extern struct Value g_TheFalseValue;
extern struct Value g_TheNilValue;

extern struct TypeInfo g_TheBaseObjectTypeInfo;
extern struct TypeInfo g_TheFunctionTypeInfo;
extern struct TypeInfo g_TheBuiltinFnTypeInfo;
extern struct TypeInfo g_TheIntegerTypeInfo;
extern struct TypeInfo g_TheRealTypeInfo;
extern struct TypeInfo g_TheStringTypeInfo;
extern struct TypeInfo g_TheBooleanTypeInfo;

extern struct TypeTable g_TheGlobalTypeTable;

extern struct SymbolTable *g_TheGlobalScope;
extern struct SymbolTable g_TheUberScope;

int GlobalsInit(void);
int GlobalsDenit(void);

#endif
