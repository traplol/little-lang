#include "../src/token.c"
#include "../src/lexer.c"

#include "c_test.h"
#include "test_helpers.h"

TEST(LexerMakeFree) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    char *filename = strdup("file.ll");
    char *code = strdup("   def hello \n world");
    assert_eq(0, LexerMake(lexer, filename, code), "LexerMake failed");
    assert_eq(0, LexerFree(lexer), "LexerFree failed");
    free(lexer);
}

TEST(LexerNextToken) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("file.ll");
    char *code = strdup("   def hello \n world");
    LexerMake(lexer, filename, code);
    assert_eq(0, LexerNextToken(lexer, &token), "LexerNextToken failed");
    assert_str_eq("def", token->TokenStr, "Did not successfully parse first token.");
    TokenFree(token);
    free(token);
    assert_eq(0, LexerNextToken(lexer, &token), "LexerNextToken failed");
    assert_str_eq("hello", token->TokenStr, "Did not successfully parse second token.");
    TokenFree(token);
    free(token);
    assert_eq(0, LexerNextToken(lexer, &token), "LexerNextToken failed");
    assert_str_eq("world", token->TokenStr, "Did not successfully parse third token.");
    TokenFree(token);
    free(token);
    LexerFree(lexer);
    free(lexer);
}

int main() {
    TEST_RUN(LexerMakeFree);
    TEST_RUN(LexerNextToken);
    return 0;
}
