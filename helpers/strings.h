#ifndef _LITTLE_LANG_HELPERS_STRINGS_H
#define _LITTLE_LANG_HELPERS_STRINGS_H

/* Returns a hash table index for a string. */
unsigned int string_hash(const char *s);

/* Duplicates a string */
char *strdup(const char *s);

#endif
