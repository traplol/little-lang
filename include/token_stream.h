#ifndef _LITTLE_LANG_TOKEN_STREAM_H
#define _LITTLE_LANG_TOKEN_STREAM_H

#include "lexer.h"

struct Node {
    struct Node *Prev;
    struct Token *Token;
    struct Node *Next;
};

struct TokenStream {
    struct Node *Head;
    struct Node *Tail;
    struct Node *Current;
    struct Lexer *Lexer;
};

/* Initializes the token stream. */
int TokenStreamMake(struct TokenStream *tokenStream, struct Lexer *lexer);
/* Frees the token stream */
int TokenStreamFree(struct TokenStream *tokenStream);

/* Creates a new node and appends it to the end of the stream. */
int TokenStreamAppend(struct TokenStream *tokenStream, struct Token *token);
/* Advances the current node to the next by one. */
int TokenStreamAdvance(struct TokenStream *tokenStream);
/* Rewinds the current node to the previous one. */
int TokenStreamRewind(struct TokenStream *tokenStream);

#endif
