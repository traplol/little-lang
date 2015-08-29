#ifndef _LITTLE_LANG_HELPERS_STRING_HASH_H
#define _LITTLE_LANG_HELPERS_STRING_HASH_H

unsigned int string_hash(char *str) {
    unsigned char *s = (unsigned char*)str;
    unsigned int hash = 5381;
    int c;
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

#endif
