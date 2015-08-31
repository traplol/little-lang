#include "../src/token.c"
#include "../src/lexer.c"

#include "strings.h"
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

#define _LEX_TEST(lex, str, tok, typ)                                   \
    assert_eq(0, LexerNextToken(lex, &tok), "LexerNextTokenFailed");    \
    assert_str_eq(str, tok->TokenStr, "Did not successfully parse '" str "' token."); \
    assert_eq(typ, tok->Type, "Did not correctly set token '" str "' type.");

#define _LEX_TOK_FREE(tok)                      \
    TokenFree(tok);                             \
    free(tok)
    

#define LEX_TEST_LC(lex, str, lin, col, tok, typ)                       \
    _LEX_TEST(lex,str,tok,typ);                                         \
    assert_eq(lin, token->LineNumber, "Line number incorrectly parsed for '" str "'."); \
    assert_eq(col, token->ColumnNumber, "Column number incorrectly parsed for '" str "'."); \
    _LEX_TOK_FREE(tok);
    

#define LEX_TEST(lex, str, tok, typ) \
    _LEX_TEST(lex, str, tok, typ) \
    _LEX_TOK_FREE(tok);

TEST(LexerNextToken) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("test.ll");
    char *code = strdup(
        "def fortytwo {\n"
        "    42\n"
        "}");
    LexerMake(lexer, filename, code);

    LEX_TEST_LC(lexer, "def", 1, 1, token, TokenDef);
    LEX_TEST_LC(lexer, "fortytwo", 1, 5, token, TokenIdentifer);
    LEX_TEST_LC(lexer, "{", 1, 14, token, TokenLeftCurlyBrace);
    LEX_TEST_LC(lexer, "42", 2, 5, token, TokenIntegerConstant);
    LEX_TEST_LC(lexer, "}", 3, 1, token, TokenRightCurlyBrace);

    LexerFree(lexer);
    free(lexer);
}


const char *SourceCode =
    " _identifier42 "
    " ident "
    " 42 "
    " 123.5 "
    " def "
    " mut "
    " const "
    " { "
    " } "
    " = "
    " + "
    " - "
    " * "
    " / "
    " \"hello \\\"world\\\"\" "
    ;
TEST(LexerTestAllTokenTypes) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("test.ll");
    char *code = strdup(SourceCode);
    LexerMake(lexer, filename, code);
    
    LEX_TEST(lexer, "_identifier42", token, TokenIdentifer);
    LEX_TEST(lexer, "ident", token, TokenIdentifer);
    LEX_TEST(lexer, "42", token, TokenIntegerConstant);
    LEX_TEST(lexer, "123.5", token, TokenRealConstant);
    LEX_TEST(lexer, "def", token, TokenDef);
    LEX_TEST(lexer, "mut", token, TokenMut);
    LEX_TEST(lexer, "const", token, TokenConst);
    LEX_TEST(lexer, "{", token, TokenLeftCurlyBrace);
    LEX_TEST(lexer, "}", token, TokenRightCurlyBrace);
    LEX_TEST(lexer, "=", token, TokenEquals);
    LEX_TEST(lexer, "+", token, TokenPlus);
    LEX_TEST(lexer, "-", token, TokenMinus);
    LEX_TEST(lexer, "*", token, TokenAsterisk);
    LEX_TEST(lexer, "/", token, TokenSlash);
    LEX_TEST(lexer, "\"hello \\\"world\\\"\"", token, TokenStringLiteral);

    LexerFree(lexer);
    free(lexer);
}

int main() {
    TEST_RUN(LexerMakeFree);
    TEST_RUN(LexerNextToken);
    TEST_RUN(LexerTestAllTokenTypes);
    return 0;
}
