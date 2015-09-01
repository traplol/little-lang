#ifndef _LITTLE_LANG_TOKEN_H
#define _LITTLE_LANG_TOKEN_H

enum TokenType {
    TokenUnknown,
    TokenIdentifer,
    TokenStringLiteral,
    TokenIntegerConstant,
    TokenRealConstant,

    TokenDef,

    TokenMut,
    TokenConst,

    TokenIf,
    TokenElse,

    TokenLeftCurlyBrace,
    TokenRightCurlyBrace,
    TokenLeftParen,
    TokenRightParen,

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
    union {
        char *String;
        double Real;
        int Integer;
    } v;
};

/* Initializes the token. */
int TokenMake(struct Token *token, enum TokenType type, char *tokenStr, char *filename, int lineNumber, int columnNumber);
/* Frees all of the token's data. */
int TokenFree(struct Token *token);

#endif
