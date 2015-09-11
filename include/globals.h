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

int GlobalsInit(void);

#endif
