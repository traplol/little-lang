#ifndef _LITTLE_LANG_TOKEN_H
#define _LITTLE_LANG_TOKEN_H

enum TokenType {
    TokenUnknown,
    TokenEOS,
    TokenSemicolon,
    TokenComma,
    
    TokenIdentifer,
    TokenStringLiteral,
    TokenIntegerConstant,
    TokenRealConstant,

    TokenDef,

    TokenMut,
    TokenConst,

    TokenFor,
    TokenWhile,
    TokenIf,
    TokenElse,
    TokenTrue,
    TokenFalse,

    TokenLeftCurlyBrace,
    TokenRightCurlyBrace,
    TokenLeftParen,
    TokenRightParen,
    TokenLeftSqBracket,
    TokenRightSqBracket,

    TokenDot,
    TokenDotDot,

    TokenEquals,
    TokenEqEq,
    TokenBang,
    TokenBangEq,
    TokenPlus,
    TokenMinus,
    TokenAsterisk,
    TokenSlash,
    TokenPercent,
    TokenCaret,
    TokenAmp,
    TokenAmpAmp,
    TokenBar,
    TokenBarBar,
    TokenLt,
    TokenLtEq,
    TokenGt,
    TokenGtEq,
    TokenStarStar,
    TokenLtLt,
    TokenGtGt,

    Token_NUM_TOKENS,
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
