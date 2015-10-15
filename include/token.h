#ifndef _LITTLE_LANG_TOKEN_H
#define _LITTLE_LANG_TOKEN_H

#include "src_loc.h"

enum TokenType {
    TokenUnknown,
    TokenEOS,
    TokenSemicolon,
    TokenNewline,
    TokenComma,
    
    TokenIdentifer,
    TokenStringLiteral,
    TokenIntegerConstant,
    TokenRealConstant,

    TokenDef,

    TokenImport,
    TokenAs,

    TokenMut,
    TokenConst,
    TokenReturn,
    TokenContinue,
    TokenBreak,

    TokenClass,

    TokenFor,
    TokenWhile,
    TokenIf,
    TokenElse,
    TokenNil,
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
    struct SrcLoc SrcLoc;
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
