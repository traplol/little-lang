#include "path_resolver.h"

#include "helpers/strings.h"

#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <libgen.h>

char *ResolvePath(char *cameFrom, char *path) {
    char *fullPath, *tmp;
    if (FileExists(path)) {
        fullPath = AbsolutePath(path);
        return fullPath;
    }
    fullPath = str_cat(cameFrom, path);
    if (FileExists(fullPath)) {
        tmp = fullPath;
        fullPath = AbsolutePath(fullPath);
        free(tmp);
        return fullPath;
    }
    fullPath = AbsolutePath(fullPath);
    if (FileExists(fullPath)) {
        return fullPath;
    }
    return NULL;
}
char *AbsolutePath(char *path) {
    char actualPath[PATH_MAX+1];
    if (!FileExists(path)) {
        return NULL;
    }
    if (!realpath(path, actualPath)) {
        return NULL;
    }
    return strdup(actualPath);
}
char *GetDirectory(char *path) {
    char *r, *s;
    int len;
    if (!path) {
        return NULL;
    }
    s = strdup(path);
    dirname(s);
    r = strdup(s);
    free(s);
    len = strlen(r);
    r = realloc(r, 2 + len);
    r[len] = '/';
    r[len+1] = '\0';
    return r;
}
int FileExists(char *path) {
    FILE *file;
    if (!path) {
        return 0;
    }
    file = fopen(path, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}
