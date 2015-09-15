#ifndef _LITTLE_LANG_HELPERS_STRINGS_H
#define _LITTLE_LANG_HELPERS_STRINGS_H

#include <stdlib.h>

/* Returns a hash table index for a string. */
unsigned int string_hash(const char *s);

/* Concatenates two strings. */
char *str_cat(char *l, char *r);

#ifndef _GNU_SOURCE
/* Duplicates a string */
char *strdup(const char *s);

/* Duplicates n characters from s */
char *strndup(const char *s, unsigned int n);

#else

/* Duplicates a string */
extern char *strdup(const char *s);

/* Duplicates n characters from s */
extern char *strndup(const char *s, size_t n);


#endif /* _GNU_SOURCE */

/* Generates a valid, deterministic identifier based on a seed. */
char *ident_generator(int seed);

#endif
