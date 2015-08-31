#include "strings.h"

#include <string.h>
#include <stdlib.h>

unsigned int string_hash(const char *str) {
    unsigned char *s = (unsigned char*)str;
    unsigned int hash = 5381;
    int c;
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

char *strdup(const char *s) {
    int len = strlen(s);
    char *r = malloc(len + 1);
    return memcpy(r, s, len + 1);
}
