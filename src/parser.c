#include "parser.h"
#include "token_stream.h"
#include "result.h"
#include "value.h"
#include "globals.h"

#include "helpers/strings.h"

#include <stdlib.h>
#include <stdio.h>

const char *tokenStrings[Token_NUM_TOKENS] = {
    [TokenSemicolon] = ";",
    [TokenComma] = ",",
    [TokenIdentifer] = "<ident>",
    [TokenDef] = "def",
    [TokenMut] = "mut",
    [TokenConst] = "const",
    [TokenFor] = "for",
    [TokenWhile] = "while",
    [TokenIf] = "if",
    [TokenElse] =  "else", 
    [TokenLeftCurlyBrace] = "{",
    [TokenRightCurlyBrace] = "}",
    [TokenLeftParen] = "(",
    [TokenRightParen] = ")",
    [TokenLeftSqBracket] = "[",
    [TokenRightSqBracket] = "]",
    [TokenDot] = ".",
    [TokenDotDot] = "..",
    [TokenEquals] = "=",
};

#define SAVE(ts, sp) sp = (ts)->Current

#define RESTORE(ts, sp) (ts)->Current = sp

#define EXPECT_NO_MSG(typ, ts)                      \
    do {                                            \
        if ((typ) != (ts)->Current->Token->Type) {  \
            return R_UnexpectedToken;               \
        }                                           \
        else {                                      \
            TokenStreamAdvance((ts));               \
        }                                           \
    } while(0)

#define EXPECT(typ, ts)                                                 \
    do {                                                                \
        if ((typ) != (ts)->Current->Token->Type) {                      \
            return ParseErrorUnexpectedTokenExpected(tokenStrings[(typ)], (ts)->Current->Token); \
        }                                                               \
        else {                                                          \
            TokenStreamAdvance((ts));                                   \
        }                                                               \
    } while(0)

#define OPT_EXPECT(typ, ts)                         \
    do {                                            \
        if ((typ) == (ts)->Current->Token->Type) {  \
            TokenStreamAdvance((ts));               \
        }                                           \
    } while(0)

#define IF_FAIL_RETURN_PARSE_ERROR(result, ts, save)                \
    do {                                                            \
        if (R_OK != (result)) {                                     \
            RESTORE((ts), (save));                                  \
            return ParseErrorUnexpectedToken((ts)->Current->Token); \
        }                                                           \
    } while(0)

int opt_expect(enum TokenType type, struct TokenStream *ts) {
    int result = ts->Current->Token && ts->Current->Token->Type == type;
    if (result) {
        TokenStreamAdvance(ts);
    }
    return result;
}

int check(enum TokenType type, struct TokenStream *ts) {
    return ts->Current->Token && ts->Current->Token->Type == type;
}

int ParseErrorUnexpectedToken(struct Token *token) {
    fprintf(stderr, "Unexpected token: '%s' at %s:%d:%d\n",
            token->TokenStr,
            token->Filename,
            token->LineNumber,
            token->ColumnNumber);
    return R_UnexpectedToken;
}

int ParseErrorUnexpectedTokenExpected(const char *expected, struct Token *token) {
    fprintf(stderr, "Unexpected token: '%s' at %s:%d:%d, expected '%s'\n",
            token->TokenStr,
            token->Filename,
            token->LineNumber,
            token->ColumnNumber,
            expected);
    return R_UnexpectedToken;
}


int ParseParamList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseArgList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseCall(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseLValue(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseAssign(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec1(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec2(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec3(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec4(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec5(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec6(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec7(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec8(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec9(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec10(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec11(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec12(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrec13(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseIfElse(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseFunction(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseWhile(struct Ast **out_ast, struct TokenStream *tokenStream);
int PareFor(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseStmt(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseStmtList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseLiteral(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseIdentifier(struct Ast **out_ast, struct TokenStream *tokenStream);

/*
 * <param-list> := <identifer>
 *              := <identifier>, <param-list>
 *              := <epsilon>
 */
int ParseParamList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *params, *param;
    result = AstMakeBlank(&params);
    while (check(TokenIdentifer, tokenStream)) {
        result = AstMakeSymbol(&param, tokenStream->Current->Token->TokenStr);
        TokenStreamAdvance(tokenStream);
        if (R_OK != result) {
            break;
        }
        AstAppendChild(params, param);
        if (!opt_expect(TokenComma, tokenStream)) {
            break;
        }
    }
    *out_ast = params;
    return result;
}

/*
 * <arg-list> := <expr>
 *            := <expr> , <arg-list>
 *            := <epsilon>
 */
int ParseArgList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *args, *arg;
    result = AstMakeBlank(&args);
    while (R_OK == (result = ParseExpr(&arg, tokenStream))) {
        AstAppendChild(args, arg);
        if (!opt_expect(TokenComma, tokenStream)) {
            break;
        }
    }
    *out_ast = args;
    return result;
}

/*
 * <call> := <lvalue> ( <arg-list> )
 */
int ParseCall(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *args = NULL;
    char *identifier = tokenStream->Current->Token->TokenStr;
    EXPECT(TokenIdentifer, tokenStream);
    EXPECT(TokenLeftParen, tokenStream);
    result = ParseArgList(&args, tokenStream);
    EXPECT(TokenRightParen, tokenStream);
    identifier = strdup(identifier);
    return AstMakeCall(out_ast, identifier, args);
}

/*
 * <lvalue> := <identifier>
 *          := <lvalue> [ <stmt> ]
 *          := <lvalue> . <lvalue>
 */
int ParseLValue(struct Ast **out_ast, struct TokenStream *tokenStream) {
    if (check(TokenIdentifer, tokenStream)) {
        return ParseIdentifier(out_ast, tokenStream);
    }
    return R_UnexpectedToken;
}

/*
 * <assign> := <lvalue> = <expr>
 */
int ParseAssign(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return R_NotYetImplemented;
}

int ParseLiteral(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Node *save;
    struct Value *value = malloc(sizeof *value); /* freed on global variable. */
    struct Token *token = tokenStream->Current->Token;
    SAVE(tokenStream, save);
    switch (token->Type) {
        default:
            goto fail_cleanup;
        case TokenIntegerConstant:
            if (R_OK != ValueMake(value,
                                  &g_TheIntegerTypeInfo,
                                  &(token->v.Integer),
                                  sizeof token->v.Integer)) {
                goto fail_cleanup;
            }
            result = AstMakeInteger(out_ast, value);
            goto success;
        case TokenRealConstant:
            if (R_OK != ValueMake(value,
                                  &g_TheRealTypeInfo,
                                  &(token->v.Real),
                                  sizeof token->v.Real)) {
                goto fail_cleanup;
            }
            result = AstMakeReal(out_ast, value);
            goto success;
        case TokenStringLiteral:
            if (R_OK != ValueMake(value,
                                  &g_TheStringTypeInfo,
                                  &(token->v.String),
                                  sizeof token->v.String)) {
                goto fail_cleanup;
            }
            result = AstMakeString(out_ast, value);
            goto success;
        case TokenTrue:
            result = AstMakeBoolean(out_ast, &g_TheTrueValue);
            goto success;
        case TokenFalse:
            result = AstMakeBoolean(out_ast, &g_TheFalseValue);
            goto success;
    }

success:
    if (TokenTrue == token->Type || TokenFalse == token->Type) {
        free(value);
    }
    TokenStreamAdvance(tokenStream);
    return result;

fail_cleanup:
    RESTORE(tokenStream, save);
    free(value);
    return ParseErrorUnexpectedToken(token);
    
}

/*
 * This is more like ident/call/lvalue?
 */
int ParseIdentifier(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Node *save;
    char *identifier = tokenStream->Current->Token->TokenStr;
    SAVE(tokenStream, save);
    EXPECT(TokenIdentifer, tokenStream);
    if (opt_expect(TokenLeftParen, tokenStream)) {
        RESTORE(tokenStream, save);
        return ParseCall(out_ast, tokenStream);
    }
    identifier = strdup(identifier);
    return AstMakeSymbol(out_ast, identifier);
}

/* TOP PRECEDENCE
 * <prec1> := <lvalue>
 *         := <literal>
 *         := <assign>
 *         := <call>
 */
int ParsePrec1(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Token *token = tokenStream->Current->Token;
    switch (token->Type) {
        default:
            return ParseLValue(out_ast, tokenStream);
        case TokenIntegerConstant:
        case TokenRealConstant:
        case TokenStringLiteral:
        case TokenTrue:
        case TokenFalse:
            return ParseLiteral(out_ast, tokenStream);
    }
}

/*
 * <prec2> := <prec1>
 *         := ! <prec1>
 *         := - <prec1>
 */
int ParsePrec2(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec1(out_ast, tokenStream);
}

/*
 * <prec3> := <prec2>
 *         := <prec2> ** <prec3>
 */
int ParsePrec3(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec2(out_ast, tokenStream);
}

/*
 * <prec4> := <prec3>
 *         := <prec3> * <prec4>
 *         := <prec3> / <prec4>
 *         := <prec3> % <prec4>
 */
int ParsePrec4(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec3(out_ast, tokenStream);
}

/*
 * <prec5> := <prec4>
 *         := <prec4> + <prec5>
 *         := <prec4> - <prec5>
 */
int ParsePrec5(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec4(out_ast, tokenStream);
}

/*
 * <prec6> := <prec5>
 *         := <prec5> << <prec6>
 *         := <prec5> >> <prec6>
 */
int ParsePrec6(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec5(out_ast, tokenStream);
}

/*
 * <prec7> := <prec6>
 *         := <prec6> & <prec7>
 */
int ParsePrec7(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec6(out_ast, tokenStream);
}

/*
 * <prec8> := <prec7>
 *         := <prec7> ^ <prec6>
 */
int ParsePrec8(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec7(out_ast, tokenStream);
}

/*
 * <prec9> := <prec8>
 *         := <prec8> | <prec9>
 */
int ParsePrec9(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec8(out_ast, tokenStream);
}

/*
 * <prec10> := <prec9>
 *          := <prec9> > <prec10>
 *          := <prec9> >= <prec10>
 *          := <prec9> < <prec10>
 *          := <prec9> <= <prec10>
 */
int ParsePrec10(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec9(out_ast, tokenStream);
}

/*
 * <prec11> := <prec10>
 *          := <prec10> == <prec11>
 *          := <prec10> != <prec11>
 */
int ParsePrec11(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec10(out_ast, tokenStream);
}

/*
 * <prec12> := <prec11>
 *          := <prec11> && <prec12>
 */
int ParsePrec12(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec11(out_ast, tokenStream);
}

/*
 * <prec13> := <prec12>
 *          := <prec12> || <prec13>
 */

int ParsePrec13(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec12(out_ast, tokenStream);
}

/*
 * <expr> := <prec13>
 */
int ParseExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    return ParsePrec13(out_ast, tokenStream);
}

/*
 * <funciton> := def <identifier> ( <param-list> ) { <stmt-list> }
 *            := def <identifier { <stmt-list> }
 */
int ParseFunction(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    char *funcName;
    struct Function *fn;
    struct Value *function;
    struct Ast *params, *body;
    struct Node *save;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenDef, tokenStream); /* def */
    funcName = tokenStream->Current->Token->TokenStr;
    EXPECT(TokenIdentifer, tokenStream); /* identifer */
    if (opt_expect(TokenLeftParen, tokenStream)) { /* ( */
        result = ParseParamList(&params, tokenStream); /* param-list */
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);
        EXPECT(TokenRightParen, tokenStream); /* ) */
    }
    else {
        params = NULL; /* no params */
    }
    EXPECT(TokenLeftCurlyBrace, tokenStream);  /* { */
    result = ParseStmtList(&body, tokenStream); /* body */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);
    EXPECT(TokenRightCurlyBrace, tokenStream); /* } */

    fn = malloc(sizeof *fn);
    fn->Name = strdup(funcName);
    fn->Params = params;
    fn->Body = body;
    function = malloc(sizeof *function);
    result = ValueMake(function, &g_TheBaseObjectTypeInfo, &fn, sizeof fn);
    if (R_OK != result) {
        free(fn);
        free(function);
        /* TODO: Cleanup the body/param list that may have been alloc'd */
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);
    }
    return AstMakeFunction(out_ast, function);
}

/*
 * <ifelse> := if <expr> { <stmt-list> }
 *          := if <expr> { <stmt-list> } else { <stmt-list> }
 *          := if <expr> { <stmt-list> } else <ifelse>
 */
int ParseIfElse(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Node *save;
    struct Ast *cond, *ifBody, *elseBody;
    cond = ifBody = elseBody = NULL;
    EXPECT_NO_MSG(TokenIf, tokenStream); /* if */
    SAVE(tokenStream, save);

    result = ParseExpr(&cond, tokenStream); /* condition */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);

    EXPECT(TokenLeftCurlyBrace, tokenStream); /* { */

    result = ParseStmtList(&ifBody, tokenStream); /* body of if */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);

    EXPECT(TokenRightCurlyBrace, tokenStream); /* } */
    if (!opt_expect(TokenElse, tokenStream)) { /* no else */
        return AstMakeIfElse(out_ast, cond, ifBody, NULL);
    }
    if (opt_expect(TokenLeftCurlyBrace, tokenStream)) { /* else { */
        result = ParseStmtList(&elseBody, tokenStream);
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);
        EXPECT(TokenRightCurlyBrace, tokenStream); /* } */
        return AstMakeIfElse(out_ast, cond, ifBody, elseBody);
    }
    EXPECT(TokenIf, tokenStream); /* else if */
    /* EXPECT eats ths if but we need it to parse a valid IfElse */
    TokenStreamRewind(tokenStream);
    result = ParseIfElse(&elseBody, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save);
    return AstMakeIfElse(out_ast, cond, ifBody, elseBody);
}

/*
 * <stmt> := <assign>
 *        := <expr>
 *        := <function>
 *        := <ifelse>
 *        := <epsilon>
 */
int ParseStmt(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Node *save;
    struct Ast *ast;
    int result;
    SAVE(tokenStream, save);

    if (check(TokenEOS, tokenStream)) {
        return R_OK;
    }

    result = ParseAssign(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseExpr(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseFunction(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseIfElse(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    return ParseErrorUnexpectedToken(tokenStream->Current->Token);
}

/*
 * <stmt-terminator> := <newline>
 *                   := ;
 *
 * <stmt-list> := <stmt>
 *             := <stmt> <stmt-terminator> <stmt-list>
 */
int ParseStmtList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *tmp;
    struct Ast *ast;

    result = AstMakeBlank(&ast);
    if (R_OK != result) {
        out_ast = NULL;
        return result;
    }
    while (R_OK == (result = ParseStmt(&tmp, tokenStream)) && tmp) {
        /* FIXME: This is wrong. This says 
         *    if true {};
         * would require the semicolon if this wasn't optional.
         * OPT_EXPECT just eats the token without processing valid syntax.
         */
        OPT_EXPECT(TokenSemicolon, tokenStream);
        if (R_OK != (result = AstAppendChild(ast, tmp))) {
            break;
        }
        if (check(TokenRightCurlyBrace, tokenStream)) {
            break;
        }
        if (check(TokenEOS, tokenStream)) {
            break;
        }

    }
    if (R_OK == result) {
        *out_ast = ast;
    }
    else {
        *out_ast = NULL;
    }
    return result;
}

/*
 * <program> := <stmt-list>
 */

int ParseTokenStream(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Token *token;
    int result;
    struct Node *_restoreNode = tokenStream->Current;
    while (TokenEOS != tokenStream->Current->Token->Type) {
        token = tokenStream->Current->Token;
        switch (token->Type) {
            default:
                result = ParseErrorUnexpectedToken(token);
                goto parse_error_cleanup;
            case TokenIdentifer:
            case TokenDef:
            case TokenMut:
            case TokenConst:
            case TokenIf:
                result = ParseStmtList(out_ast, tokenStream);
                if (R_OK != result) {
                    goto parse_error_cleanup;
                }
        }
    }
    return R_OK;

parse_error_cleanup:
    /* Something happened, restore the old token in case we're in REPL 
       so it can discard the rest of the stream. */ 
    tokenStream->Current = _restoreNode;
    return result;
}

/************************ Public Functions **************************/

int Parse(struct Ast **out_ast, struct Lexer *lexer) {
    struct TokenStream *tokenStream;
    struct Token *token;
    int result;
    if (!out_ast || !lexer) {
        return R_InvalidArgument;
    }
    tokenStream = malloc(sizeof *tokenStream);
    result = TokenStreamMake(tokenStream);
    if (R_OK != result) {
        return result;
    }

    while (R_OK == (result = LexerNextToken(lexer, &token)) && token->Type != TokenUnknown) {
        result = TokenStreamAppend(tokenStream, token);
        if (R_EndOfTokenStream == result) {
            result = R_OK;
            break;
        }
    }
    if (R_OK != result) {
        return result;
    }

    result = ParseTokenStream(out_ast, tokenStream);
    if (R_OK != result) {
        puts("Parse failed!");
        *out_ast = NULL;
        return result;
    }
    puts("Parse succesfull!");

    return TokenStreamFree(tokenStream);
}
