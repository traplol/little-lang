#ifndef _LITTLE_LANG_LEXER_H
#define _LITTLE_LANG_LEXER_H

#include "token.h"

struct Lexer {
    char *Filename;
    char *Code;
    char *Pos;
    unsigned int Length;
    int CurrentLineNumber;
    int CurrentColumnNumber;
    int REPL;
    const char *REPLPrompt;
};

int LexerMake(struct Lexer *lexer, char *filename, char *code);
int LexerFree(struct Lexer *lexer);
int LexerThrowAwayCode(struct Lexer *lexer);
int LexerNextToken(struct Lexer *lexer, struct Token **out_token);
int LexerPeekToken(struct Lexer *lexer, struct Token **out_token);

#endif
