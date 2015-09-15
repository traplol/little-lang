#ifndef _LITTLE_LANG_HELPERS_STRINGS_H
#define _LITTLE_LANG_HELPERS_STRINGS_H

/* Returns a hash table index for a string. */
unsigned int string_hash(const char *s);

/* Duplicates a string */
char *strdup(const char *s);

/* Duplicates n characters from s */
char *strndup(const char *s, unsigned int n);

/* Generates a valid, deterministic identifier based on a seed. */
char *ident_generator(int seed);

#endif
