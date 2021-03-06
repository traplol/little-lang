#include "token.h"
#include "result.h"

#include "helpers/strings.h"

#include <stdlib.h>
#include <string.h>

char escape_char(char c) {
    switch (c) {
        default: return c;
        case 't': return '\t';
        case 'n': return '\n';
        case 'r': return '\r';
    }
}


char *parse_escaped_string(char *str) {
    #define SIZE 256
    char *string, *old, tmp[SIZE];
    int i = 0;
    ++str;
    string = calloc(1,1);
    while (1) {
        if (1 + i >= SIZE) {
            tmp[i] = 0;
            old = string;
            string = str_cat(string, tmp);
            free(old);
            i = 0;
        }
        if ('"' == *str) {
            tmp[i] = 0;
            old = string;
            string = str_cat(string, tmp);
            free(old);
            break;
        }
        else if ('\\' == *str) {
            ++str;
            tmp[i++] = escape_char(*str);
        }
        else {
            tmp[i++] = *str;
        }
        ++str;
    }
    return string;
    #undef SIZE
}

void TokenSetValue(struct Token *token) {
    switch (token->Type) {
        default:
            token->v.String = NULL;
            return;
            /* TODO: use something other than strtol and strtod */
        case TokenIntegerConstant:
            token->v.Integer = strtol(token->TokenStr, NULL, 10);
            return;
        case TokenRealConstant:
            token->v.Real = strtod(token->TokenStr, NULL);
            return;
        case TokenStringLiteral:
            token->v.String = parse_escaped_string(token->TokenStr);
            return;
    }
}

int TokenMake(struct Token *token, enum TokenType type, char *tokenStr, char *filename, int lineNumber, int columnNumber) {
    if (!token || !tokenStr || !filename) {
        return R_InvalidArgument;
    }
    token->Type = type;
    token->TokenStr = strdup(tokenStr);
    token->SrcLoc.Filename = filename;
    token->SrcLoc.LineNumber = lineNumber;
    token->SrcLoc.ColumnNumber = columnNumber;
    TokenSetValue(token);
    return R_OK;
}
int TokenFree(struct Token *token) {
    if (!token) {
        return R_InvalidArgument;
    }
    if (TokenStringLiteral == token->Type) {
        free(token->v.String);
    }
    free(token->TokenStr);
    return R_OK;
}
