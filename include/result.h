#ifndef _LITTLE_LANG_RESULT_H
#define _LITTLE_LANG_RESULT_H


#define R_True -1
#define R_False 0

enum {
    __RESULT_ENUM_FORCE_SIGNEDNESS__ = -1,
    R_OK = 0,
    R_InvalidArgument,
    R_AllocFailed,
    R_KeyAlreadyInTable,
    R_KeyNotInTable,
    R_UnexpectedToken,
    R_NotYetImplemented,
    R_OperationFailed,
    R_GlobalsAlreadyInitted,
    R_EndOfTokenStream,
    R_FileNotFound,
    R_OutOfMemory,
    R_MethodNotFound,
};

/*
#define R_OK                    0
#define R_InvalidArgument       1
#define R_AllocFailed           2
#define R_KeyAlreadyInTable     3
#define R_KeyNotInTable         4
#define R_UnexpectedToken       5
#define R_NotYetImplemented     6
#define R_OperationFailed       7
#define R_GlobalsAlreadyInitted 8
#define R_EndOfTokenStream      9
#define R_FileNotFound          10
#define R_OutOfMemory           11
#define R_MethodNotFound        12
*/


#endif
