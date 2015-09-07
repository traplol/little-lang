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
    assert_eq(0, LexerNextToken(lex, &tok), "LexerNextToken Failed");    \
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
    

static int lex_tests = 0;
#define LEX_TEST(lex, str, tok, typ)            \
    lex_tests++;                                \
    _LEX_TEST(lex, str, tok, typ)               \
    _LEX_TOK_FREE(tok);

TEST(LexerNextToken) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("test.ll");
    char *code = strdup(
        "def fortytwo {\n"
        "    42;\n"
        "}");
    LexerMake(lexer, filename, code);

    LEX_TEST_LC(lexer, "def", 1, 1, token, TokenDef);
    LEX_TEST_LC(lexer, "fortytwo", 1, 5, token, TokenIdentifer);
    LEX_TEST_LC(lexer, "{", 1, 14, token, TokenLeftCurlyBrace);
    LEX_TEST_LC(lexer, "42", 2, 5, token, TokenIntegerConstant);
    LEX_TEST_LC(lexer, ";", 2, 7, token, TokenSemicolon);
    LEX_TEST_LC(lexer, "}", 3, 1, token, TokenRightCurlyBrace);

    LexerFree(lexer);
    free(lexer);
}
TEST(LexerPeekToken) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("test.ll");
    char *code = strdup(
        "def fortytwo {\n"
        "    42\n"
        "}");
    LexerMake(lexer, filename, code);

    assert_eq(0, LexerPeekToken(lexer, &token), "LexerPeekToken Failed");
    assert_str_eq("def", token->TokenStr, "Did not successfully parse 'def' token.");
    TokenFree(token);
    free(token);

    assert_eq(0, LexerPeekToken(lexer, &token), "LexerPeekToken Failed");
    assert_str_eq("def", token->TokenStr, "Peek consumed 'def' token.");
    TokenFree(token);
    free(token);

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
    " ( "
    " ) "
    " if "
    " else "
    " [ "
    " ] "
    " . "
    " .. "
    " == "
    " != "
    " % "
    " ^ "
    " & "
    " && "
    " | "
    " || "
    " < "
    " <= "
    " > "
    " >= "
    " ** "
    " << "
    " >> "
    " for "
    " while "
    " ; "
    " true "
    " false "
    " , "
    ;
TEST(LexerTestAllTokenTypes) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("test.ll");
    char *code = strdup(SourceCode);
    lex_tests = 1; /* Start this at 1 so we skip TokenUnknown. */
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
    LEX_TEST(lexer, "(", token, TokenLeftParen);
    LEX_TEST(lexer, ")", token, TokenRightParen);
    LEX_TEST(lexer, "if", token, TokenIf);
    LEX_TEST(lexer, "else", token, TokenElse);
    LEX_TEST(lexer, "[", token, TokenLeftSqBracket);
    LEX_TEST(lexer, "]", token, TokenRightSqBracket);
    LEX_TEST(lexer, ".", token, TokenDot);
    LEX_TEST(lexer, "..", token, TokenDotDot);
    LEX_TEST(lexer, "==", token, TokenEqEq);
    LEX_TEST(lexer, "!=", token, TokenBangEq);
    LEX_TEST(lexer, "%", token, TokenPercent);
    LEX_TEST(lexer, "^", token, TokenCaret);
    LEX_TEST(lexer, "&", token, TokenAmp);
    LEX_TEST(lexer, "&&", token, TokenAmpAmp);
    LEX_TEST(lexer, "|", token, TokenBar);
    LEX_TEST(lexer, "||", token, TokenBarBar);
    LEX_TEST(lexer, "<", token, TokenLt);
    LEX_TEST(lexer, "<=", token, TokenLtEq);
    LEX_TEST(lexer, ">", token, TokenGt);
    LEX_TEST(lexer, ">=", token, TokenGtEq);
    LEX_TEST(lexer, "**", token, TokenStarStar);
    LEX_TEST(lexer, "<<", token, TokenLtLt);
    LEX_TEST(lexer, ">>", token, TokenGtGt);
    LEX_TEST(lexer, "for", token, TokenFor);
    LEX_TEST(lexer, "while", token, TokenWhile);
    LEX_TEST(lexer, ";", token, TokenSemicolon);
    LEX_TEST(lexer, "true", token, TokenTrue);
    LEX_TEST(lexer, "false", token, TokenFalse);
    LEX_TEST(lexer, ",", token, TokenComma);

    assert_eq(Token_NUM_TOKENS, lex_tests, "Not all tokens have been tested.");

    LexerFree(lexer);
    free(lexer);
}

TEST(LexerEdgeCases) {
    struct Lexer *lexer = malloc(sizeof *lexer);
    struct Token *token;
    char *filename = strdup("test.ll");
    char *code = strdup(
        "(\"str in parens\")\n"
        "(;;)\n"
        "(((()))) (()())\n"
        );
    LexerMake(lexer, filename, code);

    LEX_TEST_LC(lexer, "(", 1, 1, token, TokenLeftParen);
    LEX_TEST_LC(lexer, "\"str in parens\"", 1, 2, token, TokenStringLiteral);
    LEX_TEST_LC(lexer, ")", 1, 17, token, TokenRightParen);

    LEX_TEST_LC(lexer, "(", 2, 1, token, TokenLeftParen);
    LEX_TEST_LC(lexer, ";", 2, 2, token, TokenSemicolon);
    LEX_TEST_LC(lexer, ";", 2, 3, token, TokenSemicolon);
    LEX_TEST_LC(lexer, ")", 2, 4, token, TokenRightParen);

    LEX_TEST_LC(lexer, "(", 3, 1, token, TokenLeftParen);
    LEX_TEST_LC(lexer, "(", 3, 2, token, TokenLeftParen);
    LEX_TEST_LC(lexer, "(", 3, 3, token, TokenLeftParen);
    LEX_TEST_LC(lexer, "(", 3, 4, token, TokenLeftParen);
    LEX_TEST_LC(lexer, ")", 3, 5, token, TokenRightParen);
    LEX_TEST_LC(lexer, ")", 3, 6, token, TokenRightParen);
    LEX_TEST_LC(lexer, ")", 3, 7, token, TokenRightParen);
    LEX_TEST_LC(lexer, ")", 3, 8, token, TokenRightParen);

    LEX_TEST_LC(lexer, "(", 3, 10, token, TokenLeftParen);
    LEX_TEST_LC(lexer, "(", 3, 11, token, TokenLeftParen);
    LEX_TEST_LC(lexer, ")", 3, 12, token, TokenRightParen);
    LEX_TEST_LC(lexer, "(", 3, 13, token, TokenLeftParen);
    LEX_TEST_LC(lexer, ")", 3, 14, token, TokenRightParen);
    LEX_TEST_LC(lexer, ")", 3, 15, token, TokenRightParen);
}

int main() {
    TEST_RUN(LexerMakeFree);
    TEST_RUN(LexerNextToken);
    TEST_RUN(LexerPeekToken);
    TEST_RUN(LexerTestAllTokenTypes);
    TEST_RUN(LexerEdgeCases);
    return 0;
}
