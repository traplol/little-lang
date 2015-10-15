#ifndef _LITTLE_LANG_MACRO_HELPERS_H
#define _LITTLE_LANG_MACRO_HELPERS_H

#define STR(s) #s
#define XSTR(s) STR(s)
#define GLUE2(a, b) a ## b
#define GLUE3(a, b, c) a ## b ## c

#define STRLEN_LIT(lit) ((sizeof lit)-1)

#endif
