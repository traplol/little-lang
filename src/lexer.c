#include "lexer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define STR_EQ(a,b) (0 == strcmp(a, b))
#define LEX_ADV(l)                              \
    do {                                        \
        (l)->Pos++;                             \
        (l)->CurrentColumnNumber++;             \
        if(*(l)->Pos == '\n') {                 \
            (l)->CurrentLineNumber++;           \
            (l)->CurrentColumnNumber = 0;       \
        }                                       \
    } while (0)

#define LEX_ADVE(l, e)                          \
    do {                                        \
        LEX_ADV(l);                             \
        (e++);                                  \
    } while (0)
        

/************************ Helpers ************************/
int LexerIsValid(struct Lexer *lexer) {
    return lexer && lexer->Filename && lexer->Code;
}

int LexerIsInvalid(struct Lexer *lexer) {
    return !LexerIsValid(lexer);
}

int IsWhitespace(int c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}
int IsDigit(int c) {
    return '0' <= c && c <= '9';
}
int IsHexDigit(int c) {
    return IsDigit(c) ||
        ('a' <= c && c <= 'f') ||
        ('A' <= c && c <= 'F');
}
int IsAlpha(int c) {
    return ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z');
}
int IsAlphaNum(int c) {
    return IsAlpha(c) || IsDigit(c);
}
int IsIdentStartChar(int c) {
    return c == '_' || IsAlpha(c);
}
int IsIdentChar(int c) {
    return IsIdentStartChar(c) || IsDigit(c);
}

int LexerParseString(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    end = begin = lexer->Pos;
    LEX_ADVE(lexer, end); /* Eat the beginning " */
    while (*end && *end != '"') {
        if (*end == '\\' && *(end+1) == '"') {
            LEX_ADVE(lexer, end);
        }
        LEX_ADVE(lexer, end);
    }
    LEX_ADVE(lexer, end);
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    lexer->Pos = begin;
    *out_type = TokenStringLiteral;
    *out_str = str;
    return 0;
}
int LexerParseIdentOrKeyword(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    enum TokenType type;
    end = begin = lexer->Pos;
    while (*end && IsIdentChar(*end)) {
        LEX_ADVE(lexer, end);
    }
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    if (STR_EQ(str, "def")) { type = TokenDef; }
    else if (STR_EQ(str, "mut")) { type = TokenMut; }
    else if (STR_EQ(str, "const")) { type = TokenConst; }
    else if (STR_EQ(str, "if")) { type = TokenIf; }
    else if (STR_EQ(str, "else")) { type = TokenElse; }
    else { type = TokenIdentifer; }
    lexer->Pos = begin;
    *out_type = type;
    *out_str = str;
    return 0;
}
int LexerParseNumber(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    enum TokenType type;
    int fp = 0;
    end = begin = lexer->Pos;

    while (IsDigit(*end) || *end == '.') {
        if (*end == '.') {
            LEX_ADVE(lexer, end);
            fp = 1;
            while (IsDigit(*end)) {
                LEX_ADVE(lexer, end);
            }
            goto done;
        }
        LEX_ADVE(lexer, end);
    }

done:
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    type = fp ? TokenRealConstant : TokenIntegerConstant;

    lexer->Pos = begin;
    *out_type = type;
    *out_str = str;
    return 0;
}
int LexerParseOther(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *begin, *end, *str, *tmp;
    unsigned int len;
    enum TokenType type;
    end = begin = lexer->Pos;
    while (*end && !IsAlpha(*end) && !IsDigit(*end) && !IsWhitespace(*end)) {
        LEX_ADVE(lexer, end);
    }
    len = end - begin;
    str = malloc(len+1);
    tmp = str;
    /* Copy into str */
    while (begin < end) {
        *tmp++ = *begin++;
    }
    *tmp = 0;
    if (STR_EQ("{", str)) { type = TokenLeftCurlyBrace; }
    else if (STR_EQ("}", str)) { type = TokenRightCurlyBrace; }
    else if (STR_EQ("(", str)) { type = TokenLeftParen; }
    else if (STR_EQ(")", str)) { type = TokenRightParen; }
    else if (STR_EQ("=", str)) { type = TokenEquals; }
    else if (STR_EQ("+", str)) { type = TokenPlus; }
    else if (STR_EQ("-", str)) { type = TokenMinus; }
    else if (STR_EQ("*", str)) { type = TokenAsterisk; }
    else if (STR_EQ("/", str)) { type = TokenSlash; }
    else { type = TokenUnknown; }

    lexer->Pos = begin;
    *out_type = type;
    *out_str = str;
    return 0;
}

int LexerParseThing(struct Lexer *lexer, enum TokenType *out_type, char **out_str) {
    char *p = lexer->Pos;
    if (*p == '"') {
        return LexerParseString(lexer, out_type, out_str);
    }
    else if (IsIdentStartChar(*p)) {
        return LexerParseIdentOrKeyword(lexer, out_type, out_str);
    }
    else if (IsDigit(*p)) {
        return LexerParseNumber(lexer, out_type, out_str);
    }
    return LexerParseOther(lexer, out_type, out_str);
}

struct Token *LexerGetNextToken(struct Lexer *lexer, int consumeToken) {
    struct Token *token = malloc(sizeof *token);
    int line, column;
    char *out_str, *oldPos;
    unsigned int oldCurrentLineNumber, oldCurrentColumnNumber;
    enum TokenType out_type;
    while (*lexer->Pos && IsWhitespace(*lexer->Pos)) 
        LEX_ADV(lexer);
    oldPos = lexer->Pos;
    oldCurrentLineNumber = line = lexer->CurrentLineNumber;
    oldCurrentColumnNumber = column = lexer->CurrentColumnNumber;
    if (0 != LexerParseThing(lexer, &out_type, &out_str)) {
        free(token);
        return NULL;
    }
    if (!consumeToken) {
        lexer->Pos = oldPos;
        lexer->CurrentLineNumber = oldCurrentLineNumber;
        lexer->CurrentColumnNumber = oldCurrentColumnNumber;
    }
    TokenMake(token, out_type, out_str, lexer->Filename, line, column);
    return token;
}

int LexerSharedGetNext(struct Lexer *lexer, struct Token **out_token, int consume) {
    struct Token *token;
    if (LexerIsInvalid(lexer)) {
        return -1;
    }
    token = LexerGetNextToken(lexer, consume);
    if (!out_token) {
        TokenFree(token);
        free(token);
        return -1;
    }
    *out_token = token;
    return 0;
}

/*********************** Public Functions ************************/

int LexerMake(struct Lexer *lexer, char *filename, char *code) {
    if (!lexer || !filename) {
        return -1;
    }
    lexer->Filename = filename;
    lexer->Code = code;
    lexer->Pos = lexer->Code;
    lexer->Length = strlen(lexer->Code);
    lexer->CurrentLineNumber = 1;
    lexer->CurrentColumnNumber = 1;
    return 0;
}

int LexerFree(struct Lexer *lexer) {
    if (LexerIsInvalid(lexer)) {
        return -1;
    }
    free(lexer->Filename);
    free(lexer->Code);
    return 0;
}

int LexerNextToken(struct Lexer *lexer, struct Token **out_token) {
    return LexerSharedGetNext(lexer, out_token, 1);
}

int LexerPeekToken(struct Lexer *lexer, struct Token **out_token) {
    return LexerSharedGetNext(lexer, out_token, 0);
}
