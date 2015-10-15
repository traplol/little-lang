#include "llstring.h"

#include "result.h"
#include "helpers/strings.h"

#include <string.h>
#include <stdlib.h>

int LLStringMake(struct LLString *string, char *cString) {
    char *s;
    if (!string || !cString) {
        return R_InvalidArgument;
    }
    string->Length = strlen(cString);
    string->CString = malloc(1 + string->Length);
    s = string->CString;
    while (*cString) {
        *s++ = *cString++;
    }
    *s = 0;
    return R_OK;
}
int LLStringFree(struct LLString *string) {
    if (!string) {
        return R_InvalidArgument;
    }
    string->Length = 0;
    free(string->CString);
    string->CString = NULL;
    return R_OK;
}

int LLStringCharAt(struct LLString *string, unsigned int idx, struct LLString **out_string) {
    struct LLString *out;
    char c[2];
    if (!string || !out_string) {
        return R_InvalidArgument;
    }
    if (idx >= string->Length) {
        return R_InvalidArgument;
    }
    c[0] = string->CString[idx];
    c[1] = 0;
    out = malloc(sizeof *out);
    LLStringMake(out, c);
    *out_string = out;
    return R_OK;
}
int LLStringConcatenate(struct LLString *s1, struct LLString *s2, struct LLString **out_string) {
    struct LLString *out;
    char *s, *l, *r;
    if (!s1 || !s2 || !out_string) {
        return R_InvalidArgument;
    }
    out = malloc(sizeof *out);
    /* TODO: overflow? */
    out->Length = s1->Length + s2->Length;
    out->CString = malloc(1 + out->Length);

    s = out->CString;
    l = s1->CString;
    r = s2->CString;
    while (*l) {
        *s++ = *l++;
    }
    while (*r) {
        *s++ = *r++;
    }
    *s = 0;
    return R_OK;
}
int LLStringSlice(struct LLString *string, unsigned int start, unsigned int end, struct LLString **out_string) {
    struct LLString *out;
    char *s, *o;
    if (!string || !out_string) {
        return R_InvalidArgument;
    }
    if (end < start) {
        return R_InvalidArgument;
    }
    if (start >= string->Length) {
        return R_InvalidArgument;
    }
    if (end >= string->Length) {
        return R_InvalidArgument;
    }
    out = malloc(sizeof *out);
    out->Length = end - start;
    out->CString = malloc(1 + out->Length);

    s = out->CString;
    while (start <= end) {
        *s++ = string->CString[start++];
    }
    *s = 0;
    return R_OK;
}
