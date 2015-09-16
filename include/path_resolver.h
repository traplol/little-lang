#ifndef _LITTLE_LANG_PATH_RESOLVER_H
#define _LITTLE_LANG_PATH_RESOLVER_H

char *ResolvePath(char *cameFrom, char *path);
char *AbsolutePath(char *path);
char *GetDirectory(char *path);
int FileExists(char *path);

#endif
