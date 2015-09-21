#include "../helpers/strings.c"
#include "../src/lexer.c"
#include "../src/token.c"
#include "../src/token_stream.c"
#include "result.h"

#include "c_test.h"
#include "test_helpers.h"

#include <stdlib.h>

static struct Lexer *lex;
void setup() {
    lex = calloc(sizeof *lex, 1);
}
void done() {
    free(lex);
}

TEST(TokenStreamMakeFree) {
    struct TokenStream *ts = malloc(sizeof *ts);
    assert_eq(R_OK, TokenStreamMake(ts, lex), "TokenStreamMake failed");
    assert_eq(R_OK, TokenStreamFree(ts), "TokenStreamFree failed");
    free(ts);
}

TEST(TokenStreamAppend) {
    struct TokenStream *ts = malloc(sizeof *ts);
    struct Token *def = malloc(sizeof *def);
    struct Token *ident = malloc(sizeof *ident);
    struct Token *lbrace = malloc(sizeof *lbrace);
    TokenMake(def, TokenDef, "def", "test.ll", 5, 10);
    TokenMake(ident, TokenIdentifer, "hello_world", "test.ll", 5, 10);
    TokenMake(lbrace, TokenLeftCurlyBrace, "{", "test.ll", 4, 2);
    TokenStreamMake(ts, lex);
    assert_eq(R_OK, TokenStreamAppend(ts, def), "Failed to append token.");
    assert_eq(def, ts->Head->Token, "Did not correctly assign head.");
    assert_eq(def, ts->Tail->Token, "Did not correctly assign tail.");
    assert_eq(def, ts->Current->Token, "Did not correctly assign current.");

    assert_eq(R_OK, TokenStreamAppend(ts, ident), "Failed to append token.");
    assert_eq(def, ts->Head->Token, "Head should still be 'def'.");
    assert_eq(ident, ts->Tail->Token, "Did not correctly assign tail.");
    assert_eq(def, ts->Current->Token, "Current should still be 'def'.");

    assert_eq(R_OK, TokenStreamAppend(ts, lbrace), "Failed to append token.");
    assert_eq(def, ts->Head->Token, "Head should still be 'def'.");
    assert_eq(lbrace, ts->Tail->Token, "Did not correctly assign tail.");
    assert_eq(def, ts->Current->Token, "Current should still be 'def'.");

    TokenStreamFree(ts);
    free(def);
    free(ident);
    free(lbrace);
    free(ts);
}


TEST(TokenStreamAdvanceRewind) {
    struct TokenStream *ts = malloc(sizeof *ts);
    struct Token *def = malloc(sizeof *def);
    struct Token *ident = malloc(sizeof *ident);
    struct Token *lbrace = malloc(sizeof *lbrace);
    TokenMake(def, TokenDef, "def", "test.ll", 5, 10);
    TokenMake(ident, TokenIdentifer, "hello_world", "test.ll", 5, 10);
    TokenMake(lbrace, TokenLeftCurlyBrace, "{", "test.ll", 4, 2);
    TokenStreamMake(ts, lex);
    TokenStreamAppend(ts, def);
    TokenStreamAppend(ts, ident);
    TokenStreamAppend(ts, lbrace);

    assert_eq(R_OK, TokenStreamAdvance(ts), "TokenStreamAdvance failed.");
    assert_eq(ident, ts->Current->Token, "Did not correctly advance stream.");
    assert_eq(R_OK, TokenStreamRewind(ts), "TokenStreamAdvance failed.");
    assert_eq(def, ts->Current->Token, "Did not correctly advance stream.");
    assert_eq(R_OK, TokenStreamAdvance(ts), "TokenStreamAdvance failed.");
    assert_eq(ident, ts->Current->Token, "Did not correctly advance stream.");
    assert_eq(R_OK, TokenStreamAdvance(ts), "TokenStreamAdvance failed.");
    assert_eq(lbrace, ts->Current->Token, "Did not correctly advance stream.");

    // Test extra advances/rewinds.
    while (ts->Current->Next) {
        TokenStreamAdvance(ts);
    }
    assert_eq(R_OperationFailed, TokenStreamAdvance(ts), "TokenStreamAdvance should have failed.");

    while (ts->Current->Prev) {
        TokenStreamRewind(ts);
    }
    assert_eq(R_OperationFailed, TokenStreamRewind(ts), "TokenStreamRewind should have failed.");

    TokenStreamFree(ts);
    free(def);
    free(ident);
    free(lbrace);
    free(ts);
}

int main() {
    setup();
    TEST_RUN(TokenStreamMakeFree);
    TEST_RUN(TokenStreamAppend);
    TEST_RUN(TokenStreamAdvanceRewind);
    done();
}
