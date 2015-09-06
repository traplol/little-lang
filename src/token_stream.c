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

/*********************** Public Functions ***********************/

int TokenStreamMake(struct TokenStream *tokenStream) {
    if (!tokenStream) {
        return R_InvalidArgument;
    }
    tokenStream->Head = NULL;
    tokenStream->Tail = tokenStream->Head;
    tokenStream->Current = tokenStream->Head;
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
    return R_OK;
}
int TokenStreamAdvance(struct TokenStream *tokenStream) {
    if (TokenStreamIsInvalid(tokenStream)) {
        return R_InvalidArgument;
    }
    if (!tokenStream->Current || !tokenStream->Current->Next) {
        return R_OperationFailed;
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
