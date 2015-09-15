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

char *str_cat(char *l, char *r) {
    int len = strlen(l) + strlen(r);
    char *s = malloc(len + 1);
    while(*l) {
        *s++ = *l++;
    }
    while(*r) {
        *s++ = *r++;
    }

    *s = 0;
    return s-len;
}

#ifndef _GNU_SOURCE
char *strdup(const char *s) {
    unsigned int len = strlen(s);
    char *r = malloc(len + 1);
    return memcpy(r, s, len + 1);
}

char *strndup(const char *s, unsigned int n) {
    unsigned int tmpLen = strlen(s);
    unsigned int len = tmpLen > n ? n : tmpLen;
    char *r = malloc(len + 1);
    r[len] = '\0';
    return memcpy(r, s, len);
}
#endif /* _GNU_SOURCE */

char get_random_uppercase() {
    int r = rand() % 26;
    return r + 'A';
}
char get_random_lowercase() {
    int r = rand() % 26;
    return r + 'a';
}
char get_random_number() {
    int r = rand() % 10;
    return r + '0';
}

char *ident_generator(int seed) {
    int len, i;
    char *str, c;
    srand(seed);
    len = rand() % 20 + 5; /* 5 - 25 chars long. */
    str = malloc(len+1);
    switch (rand() % 10) {
        case 0: case 1: case 2: case 3: case 4:
            /* 50% chance at being a lowercase char */
            c = get_random_lowercase();
            break;
        case 5: case 6: case 7: case 8:
            /* 40% chance at being a uppercase char */
            c = get_random_uppercase();
            break;
        case 9: 
            /* 10% chance of at being a underscore */
            c = '_';
            break;
    }
    str[0] = c;
    for (i = 1; i < len; ++i) {
        switch (rand() % 10) {
            case 0: case 1: case 2: case 3: case 4:
                /* 50% chance at being a lowercase char */
                c = get_random_lowercase();
                break;
            case 5: case 6: case 7:
                /* 30% chance at being a uppercase char */
                c = get_random_uppercase();
                break;
            case 8:
                /* 10% chance at being a number char */
                c = get_random_number();
                break;
            case 9:
                /* 10% chance of at being a underscore */
                c = '_';
                break;
        }
        str[i] = c;
    }
    str[i] = 0;
    return str;
}
