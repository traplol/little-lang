#ifndef _LITTLE_LANG_TOKEN_H
#define _LITTLE_LANG_TOKEN_H

enum TokenType {
    TokenIdentifer,
    TokenNumberConstant,

    TokenDef,
    TokenEnd,

    TokenMut,
    TokenConst,

    TokenEquals,
    TokenPlus,
    TokenMinus,
    TokenAsterisk,
    TokenSlash,
};

struct Token {
    enum TokenType Type;
    char *TokenStr;
    char *Filename;
    int LineNumber;
    int ColumnNumber;
    struct Token *Next;
};

/* Initializes the token. */
int TokenMake(struct Token *token, enum TokenType type, char *tokenStr, char *filename, int lineNumber, int columnNumber);
/* Frees all of the token's data. */
int TokenFree(struct Token *token);

#endif
