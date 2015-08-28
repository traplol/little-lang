#ifndef _LITTLE_LANG_TESTS_TEST_HELPERS_H
#define _LITTLE_LANG_TESTS_TEST_HELPERS_H

#include <string.h>
#include <stdlib.h>

char *strdup(const char *s) {
    int len = strlen(s);
    char *r = malloc(len + 1);
    return memcpy(r, s, len + 1);
}

#endif
