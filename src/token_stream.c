#include "token_stream.h"
#include "token.h"
#include "result.h"

#include <stdlib.h>

/*********************** Helpers ************************/

int TokenStreamIsValid(struct TokenStream *tokenStream) {
    return NULL != tokenStream;
}

int TokenStreamIsInvalid(struct TokenStream *tokenStream) {
    return !TokenStreamIsValid(tokenStream);
}

struct Node *NodeMake(struct Token *token) {
    struct Node *node = malloc(sizeof *node);
    node->Prev = NULL;
    node->Token = token;
    node->Next = NULL;
    return node;
}

void NodeFree(struct Node *node) {
    TokenFree(node->Token);
    free(node);
}

int TokenStreamREPLNextToken(struct TokenStream *tokenStream) {
    int result;
    struct Token *token; 
    result = LexerNextToken(tokenStream->Lexer, &token);
    if (R_OK != result) {
        return result;
    }
    result = TokenStreamAppend(tokenStream, token);
    if (R_OK != result) {
        return result;
    }
    return R_OK;
}

int TokenStreamAddExistingTokens(struct TokenStream *tokenStream) {
    int result;
    struct Token *token;
    while (R_OK == (result = LexerNextToken(tokenStream->Lexer, &token))) {
        result = TokenStreamAppend(tokenStream, token);
        if (R_EndOfTokenStream == result) {
            result = R_OK;
            break;
        }
    }
    return result;
}

/*********************** Public Functions ***********************/

int TokenStreamMake(struct TokenStream *tokenStream, struct Lexer *lexer) {
    if (!tokenStream) {
        return R_InvalidArgument;
    }
    tokenStream->Head = NULL;
    tokenStream->Tail = tokenStream->Head;
    tokenStream->Current = tokenStream->Head;
    tokenStream->Lexer = lexer;
    if (lexer->Length > 0) {
        TokenStreamAddExistingTokens(tokenStream);
    }
    else if (lexer->REPL) { /* Setup the first token. */
        TokenStreamAdvance(tokenStream);
    }
    return R_OK;
}

int TokenStreamFree(struct TokenStream *tokenStream) {
    struct Node *head, *next;
    if (TokenStreamIsInvalid(tokenStream)) {
        return R_InvalidArgument;
    }
    head = tokenStream->Head;
    while (head) {
        next = head->Next;
        NodeFree(head);
        head = next;
    }
    tokenStream->Head = NULL;
    tokenStream->Current = NULL;
    tokenStream->Tail = NULL;
    return R_OK;
}

int TokenStreamAppend(struct TokenStream *tokenStream, struct Token *token) {
    struct Node *newNode;
    if (TokenStreamIsInvalid(tokenStream) || !token) {
        return R_InvalidArgument;
    }

    newNode = NodeMake(token);
    if (!tokenStream->Head) {
        tokenStream->Head = newNode;
        tokenStream->Tail = tokenStream->Head;
        tokenStream->Current = tokenStream->Head;
    }
    else {
        tokenStream->Tail->Next = newNode;
        newNode->Prev = tokenStream->Tail;
        tokenStream->Tail = newNode;
    }
    if (TokenEOS == token->Type) {
        return R_EndOfTokenStream;
    }
    return R_OK;
}
int TokenStreamAdvance(struct TokenStream *tokenStream) {
    int result, firstTime;
    if (TokenStreamIsInvalid(tokenStream)) {
        return R_InvalidArgument;
    }
    if (!tokenStream->Current || !tokenStream->Current->Next) {
        if (tokenStream->Lexer->REPL) {
            firstTime = !tokenStream->Head;
            result = TokenStreamREPLNextToken(tokenStream);
            if (firstTime) {
                return result;
            }
        }
        else {
            return R_OperationFailed;
        }
    }
    tokenStream->Current = tokenStream->Current->Next;
    return R_OK;
}
int TokenStreamRewind(struct TokenStream *tokenStream) {
    if (TokenStreamIsInvalid(tokenStream)) {
        return R_InvalidArgument;
    }
    if (!tokenStream->Current || !tokenStream->Current->Prev) {
        return R_OperationFailed;
    }
    tokenStream->Current = tokenStream->Current->Prev;
    return R_OK;
}
