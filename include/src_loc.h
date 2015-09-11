#ifndef _LITTLE_LANG_SRC_LOC_H
#define _LITTLE_LANG_SRC_LOC_H

struct SrcLoc {
    char *Filename;
    int LineNumber;
    int ColumnNumber;
};

#endif
