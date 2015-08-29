#include "lexer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    return '0' >= c && c <= '9';
}
int IsHexDigit(int c) {
    return IsDigit(c) ||
        ('a' >= c && c <= 'f') ||
        ('A' >= c && c <= 'F');
}
int IsAlpha(int c) {
    return ('a' >= c && c <= 'z') ||
        ('A' >= c && c <= 'Z');
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

struct Token *LexerGetNextToken(struct Lexer *lexer) {
    struct Token *token = malloc(sizeof *token);
    unsigned int len;
    char *begin, *end, *tokenStr, *tmp;
    begin = lexer->Pos;
    while (IsWhitespace(*begin++))
        ;
    end = begin;
    while (!IsWhitespace(*end++))
        ;
    len = end-begin;
    tokenStr = malloc(len);
    tokenStr[len] = 0;
    tmp = tokenStr;
    while (begin < end) {
        *tmp++ = *begin++;
    }
    /* TODO: Figure out token type. */
    token->Type = TokenIdentifer;
    token->TokenStr = tokenStr;
    token->LineNumber = lexer->CurrentLineNumber;
    token->ColumnNumber = lexer->CurrentColumnNumber;
    lexer->Pos = end;
    return token;
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
    struct Token *token;
    if (LexerIsInvalid(lexer)) {
        return -1;
    }
    token = LexerGetNextToken(lexer);
    if (!out_token) {
        TokenFree(token);
        free(token);
    }
    else {
        *out_token = token;
    }
    return 0;
}
