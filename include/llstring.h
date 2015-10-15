#ifndef _LITTLE_LANG_LLSTRING_H
#define _LITTLE_LANG_LLSTRING_H

struct LLString {
    unsigned int Length;
    char *CString;
};

int LLStringMake(struct LLString *string, char *cString);
int LLStringFree(struct LLString *string);

int LLStringCharAt(struct LLString *string, unsigned int idx, struct LLString **out_string);
int LLStringConcatenate(struct LLString *s1, struct LLString *s2, struct LLString **out_string);
int LLStringSlice(struct LLString *string, unsigned int start, unsigned int end, struct LLString **out_string);

#endif
