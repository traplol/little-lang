#include "token.h"

#include <stdlib.h>


int TokenMake(struct Token *token, enum TokenType type, char *tokenStr, char *filename, int lineNumber, int columnNumber) {
    if (!token || !tokenStr || !filename) {
        return -1;
    }
    token->Type = type;
    token->TokenStr = tokenStr;
    token->Filename = filename;
    token->LineNumber = lineNumber;
    token->ColumnNumber = columnNumber;
    token->Next = NULL;
    return 0;
}
int TokenFree(struct Token *token) {
    if (!token) {
        return -1;
    }
    free(token->TokenStr);
    free(token->Filename);
    return 0;
}
