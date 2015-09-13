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

#define IF_FAIL_RETURN_PARSE_ERROR(result, ts, save, out_ast)       \
    do {                                                            \
        if (R_OK != (result)) {                                     \
            RESTORE((ts), (save));                                  \
            *(out_ast) = NULL;                                      \
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
            token->SrcLoc.Filename,
            token->SrcLoc.LineNumber,
            token->SrcLoc.ColumnNumber);
    return R_UnexpectedToken;
}

int ParseErrorUnexpectedTokenExpected(const char *expected, struct Token *token) {
    fprintf(stderr, "Unexpected token: '%s' at %s:%d:%d, expected '%s'\n",
            token->TokenStr,
            token->SrcLoc.Filename,
            token->SrcLoc.LineNumber,
            token->SrcLoc.ColumnNumber,
            expected);
    return R_UnexpectedToken;
}

int GetBinaryOperatorPrecedence(struct Token *token) {
    switch (token->Type) {
        default: return -1;
        case TokenBang:
            return 80;
        case TokenStarStar:
            return 70;
        case TokenAsterisk:
        case TokenSlash:
        case TokenPercent:
            return 60;
        case TokenPlus: 
        case TokenMinus:
            return 50;
        case TokenLtLt:
        case TokenGtGt:
            return 40;
        case TokenAmp:
            return 37;
        case TokenCaret:
            return 36;
        case TokenBar:
            return 35;
        case TokenGt:
        case TokenGtEq:
        case TokenLt:
        case TokenLtEq:
            return 30;
        case TokenEqEq:
        case TokenBangEq:
            return 20;
        case TokenAmpAmp:
            return 16;
        case TokenBarBar:
            return 15;
        case TokenEquals:
            return 10;
    }
}

enum AstNodeType GetBinaryOperatorType(struct Token *token) {
    switch (token->Type) {
        default: return UNASSIGNED;
        case TokenStarStar: return BPowExpr;
        case TokenAsterisk: return BMulExpr;
        case TokenSlash: return BDivExpr;
        case TokenPercent: return BModExpr;
        case TokenPlus: return BAddExpr;
        case TokenMinus: return BSubExpr;
        case TokenLtLt: return BLShift;
        case TokenGtGt: return BRShift;
        case TokenAmp: return BArithAndExpr;
        case TokenCaret: return BArithXorExpr;
        case TokenBar: return BArithOrExpr;
        case TokenGt: return BLogicGtExpr;
        case TokenGtEq: return BLogicGtEqExpr;
        case TokenLt: return BLogicLtExpr;
        case TokenLtEq: return BLogicLtEqExpr;
        case TokenEqEq: return BLogicEqExpr;
        case TokenBangEq: return BLogicNotEqExpr;
        case TokenAmpAmp: return BLogicAndExpr;
        case TokenBarBar: return BLogicOrExpr;
        case TokenEquals: return AssignExpr;
    }
}

int IsBinaryOperator(struct Token *token) {
    switch(token->Type) {
        default:
            return 0;
        case TokenEqEq:
        case TokenBangEq:
        case TokenPlus:
        case TokenMinus:
        case TokenAsterisk:
        case TokenSlash:
        case TokenPercent:
        case TokenCaret:
        case TokenAmp:
        case TokenAmpAmp:
        case TokenBar:
        case TokenBarBar:
        case TokenLt:
        case TokenLtEq:
        case TokenGt:
        case TokenGtEq:
        case TokenStarStar:
        case TokenLtLt:
        case TokenGtGt:
            return 1;
    }
}

enum AstNodeType GetUnaryOperatorType(struct Token *token) {
    switch(token->Type) {
        default: return UNASSIGNED;
        case TokenMinus: return UNegExpr;
        case TokenBang: return ULogicNotExpr;
    }
}

int IsUnaryOperator(struct Token *token) {
    switch(token->Type) {
        default:
            return 0;
        case TokenMinus:
        case TokenBang:
            return 1;
    }
}


int ParseParamList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseArgList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseCall(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseLValue(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseAssign(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseIfElse(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseFunction(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseWhile(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseFor(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseStmt(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseStmtList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseLiteral(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseIdentifier(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseParenExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseTerm(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseBinaryRhs(struct Ast **out_ast, struct TokenStream *tokenStream, int prec, struct Ast *lhs);
int ParseBinaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseConst(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseMut(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseCommaSeparatedIdentifiers(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseCommaSeparatedExprs(struct Ast **out_ast, struct TokenStream *tokenStream);

/* <const-expr> := const <identifier> = <expr> */
int ParseConst(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Validate 'const-expr' */
    int result;
    char *identifier;
    struct Ast *expr;
    struct Node *save;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenConst, tokenStream);
    identifier = tokenStream->Current->Token->TokenStr;
    EXPECT(TokenIdentifer, tokenStream);
    EXPECT(TokenEquals, tokenStream);
    result = ParseExpr(&expr, tokenStream);
    identifier = strdup(identifier);
    return AstMakeConst(out_ast, identifier, expr, save->Token->SrcLoc);
}

/* <comma-separated-identifers> := <identifier>
 *                              := <identifier> , <comma-separated-identifiers>
 */
int ParseCommaSeparatedIdentifiers(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *names, *curName;
    AstMakeBlank(&names);
    while(R_OK == (result = ParseIdentifier(&curName, tokenStream))) {
        AstAppendChild(names, curName);
        if (!opt_expect(TokenComma, tokenStream)) {
            break;
        }
    }
    if (R_OK == result) {
        *out_ast = names;
        return R_OK;
    }
    AstFree(names);
    *out_ast = NULL;
    return result;
}

/* <comma-separated-exprs> := <expr>
 *                         := <expr> , <comma-separated-exprs>
 */
int ParseCommaSeparatedExprs(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *exprs, *curExpr;
    AstMakeBlank(&exprs);
    while(R_OK == (result = ParseExpr(&curExpr, tokenStream))) {
        AstAppendChild(exprs, curExpr);
        if (!opt_expect(TokenComma, tokenStream)) {
            break;
        }
        if (opt_expect(TokenSemicolon, tokenStream)) {
            break;
        }
    }
    if (R_OK == result) {
        *out_ast = exprs;
        return R_OK;
    }
    AstFree(exprs);
    *out_ast = NULL;
    return result;
    
}

/* <mut-expr> := mut <comma-separated-identifiers> = <comma-separated-exprs> */
int ParseMut(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Validate 'mut-expr' */
    int result;
    struct Ast *names, *exprs;
    struct Node *save;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenMut, tokenStream);
    result = ParseCommaSeparatedIdentifiers(&names, tokenStream);
    EXPECT(TokenEquals, tokenStream);
    result = ParseCommaSeparatedExprs(&exprs, tokenStream);
    return AstMakeMut(out_ast, names, exprs, save->Token->SrcLoc);
}

/* <for> := for <expr> ; <expr> ; <expr> { <stmt-list> } */
int ParseFor(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Validate 'for' parsing. */
    int result;
    struct Ast *pre, *cond, *post, *body;
    struct Node *save;
    SAVE(tokenStream, save);

    EXPECT_NO_MSG(TokenFor, tokenStream);

    result = ParseExpr(&pre, tokenStream);
    EXPECT(TokenSemicolon, tokenStream);

    result = ParseExpr(&cond, tokenStream);
    EXPECT(TokenSemicolon, tokenStream);

    result = ParseExpr(&post, tokenStream);

    EXPECT(TokenLeftCurlyBrace, tokenStream);
    result = ParseStmtList(&body, tokenStream);
    EXPECT(TokenRightCurlyBrace, tokenStream);

    return AstMakeFor(out_ast, pre, cond, body, post, save->Token->SrcLoc);
}

/* <while> := while <expr> { <stmt-list> } */
int ParseWhile(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Validate 'while' parsing. */
    int result;
    struct Ast *cond, *body;
    struct Node *save;
    SAVE(tokenStream, save);

    EXPECT_NO_MSG(TokenWhile, tokenStream);

    result = ParseExpr(&cond, tokenStream);

    EXPECT(TokenLeftCurlyBrace, tokenStream);
    result = ParseStmtList(&body, tokenStream);
    EXPECT(TokenRightCurlyBrace, tokenStream);

    return AstMakeWhile(out_ast, cond, body, save->Token->SrcLoc);
}

/*
 * <param-list> := <identifer>
 *              := <identifier>, <param-list>
 *              := <epsilon>
 */
int ParseParamList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *params, *param;
    result = AstMakeBlank(&params); /* FIXME: Do something with result. */
    while (check(TokenIdentifer, tokenStream)) {
        result = AstMakeSymbol(&param,
                               tokenStream->Current->Token->TokenStr,
                               tokenStream->Current->Token->SrcLoc);
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
    result = AstMakeBlank(&args); /* FIXME: Do something with result. */
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
    struct Node *save;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenIdentifer, tokenStream);
    EXPECT_NO_MSG(TokenLeftParen, tokenStream);
    result = ParseArgList(&args, tokenStream); /* FIXME: Do something with result. */
    EXPECT(TokenRightParen, tokenStream);
    identifier = strdup(identifier);
    return AstMakeCall(out_ast, identifier, args, save->Token->SrcLoc);
}

/*
 * <lvalue> := <identifier>
 *          := <lvalue> [ <expr> ]
 *          := <lvalue> . <lvalue>
 */
int ParseLValue(struct Ast **out_ast, struct TokenStream *tokenStream) {
    if (check(TokenIdentifer, tokenStream)) {
        return ParseIdentifier(out_ast, tokenStream);
    }
    /* TODO: Implemented the rest of lvalue parsing. */
    return R_UnexpectedToken;
}

/*
 * <assign> := <lvalue> = <expr>
 */
int ParseAssign(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *lvalue, *expr;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseLValue(&lvalue, tokenStream);
    if (R_OK != result) {
        return result;
    }
    EXPECT_NO_MSG(TokenEquals, tokenStream);
    result = ParseExpr(&expr, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeAssign(out_ast, lvalue, expr, save->Token->SrcLoc);
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
            if (R_OK != ValueMakeInteger(value, token->v.Integer)) {
                goto fail_cleanup;
            }
            result = AstMakeInteger(out_ast, value, save->Token->SrcLoc);
            goto success;
        case TokenRealConstant:
            if (R_OK != ValueMakeReal(value, token->v.Real)) {
                goto fail_cleanup;
            }
            result = AstMakeReal(out_ast, value, save->Token->SrcLoc);
            goto success;
        case TokenStringLiteral:
            if (R_OK != ValueMakeLLString(value, token->v.String)) {
                goto fail_cleanup;
            }
            result = AstMakeString(out_ast, value, save->Token->SrcLoc);
            goto success;
        case TokenTrue:
            result = AstMakeBoolean(out_ast, &g_TheTrueValue, save->Token->SrcLoc);
            goto success;
        case TokenFalse:
            result = AstMakeBoolean(out_ast, &g_TheFalseValue, save->Token->SrcLoc);
            goto success;
        case TokenNil:
            result = AstMakeBoolean(out_ast, &g_TheNilValue, save->Token->SrcLoc);
            goto success;
    }

success:
    if (TokenTrue == token->Type || TokenFalse == token->Type || TokenNil == token->Type) {
        free(value);
    }
    TokenStreamAdvance(tokenStream);
    return result;

fail_cleanup:
    RESTORE(tokenStream, save);
    free(value);
    return R_UnexpectedToken;
}

int ParseIdentifier(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Node *save;
    char *identifier = tokenStream->Current->Token->TokenStr;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenIdentifer, tokenStream);
    identifier = strdup(identifier);
    return AstMakeSymbol(out_ast, identifier, save->Token->SrcLoc);
}

/* <paren-expr> := ( <expr> ) */
int ParseParenExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *expr;
    EXPECT_NO_MSG(TokenLeftParen, tokenStream);
    result = ParseExpr(&expr, tokenStream);
    EXPECT(TokenRightParen, tokenStream);
    *out_ast = expr;
    return result;
}

/* <unary-expr> := <unary-op> <term> */
int ParseUnaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType unOp;
    struct Ast *expr;
    struct Node *save;
    SAVE(tokenStream, save);
    if (!IsUnaryOperator(tokenStream->Current->Token)) {
        return R_UnexpectedToken;
    }
    unOp = GetUnaryOperatorType(tokenStream->Current->Token);
    TokenStreamAdvance(tokenStream);
    result = ParseTerm(&expr, tokenStream);
    if (R_OK == result) {
        return AstMakeUnaryOp(out_ast, unOp, expr, save->Token->SrcLoc);
    }
    *out_ast = NULL;
    return result;
}


/* <term> := <literal>
 *        := <lvalue>
 *        := <call>
 *        := <unary-expr>
 *        := <paren-expr>
 */
int ParseTerm(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *term;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseLiteral(&term, tokenStream);
    if (R_OK == result) {
        *out_ast = term;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    if (TokenLeftParen == tokenStream->Current->Next->Token->Type) {
        result = ParseCall(&term, tokenStream);
        if (R_OK == result) {
            *out_ast = term;
            return R_OK;
        }
    }
    else {
        result = ParseLValue(&term, tokenStream);
        if (R_OK == result) {
            *out_ast = term;
            return R_OK;
        }
    }
    RESTORE(tokenStream, save);
    result = ParseParenExpr(&term, tokenStream);
    if (R_OK == result) {
        *out_ast = term;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseUnaryExpr(&term, tokenStream);
    if (R_OK == result) {
        *out_ast = term;
        return R_OK;
    }
    RESTORE(tokenStream, save)

;
    *out_ast = NULL;
    return R_UnexpectedToken;
}

/* The right hand side of a binary expression. */
int ParseBinaryRhs(struct Ast **out_ast, struct TokenStream *tokenStream, int prec, struct Ast *lhs) {
    int result, tokenPrec, nextPrec;
    enum AstNodeType binOp;
    struct Ast *rhs;
    while (1) {
        tokenPrec = GetBinaryOperatorPrecedence(tokenStream->Current->Token);
        if (tokenPrec < prec) {
            *out_ast = lhs;
            return R_OK;
        }
        binOp = GetBinaryOperatorType(tokenStream->Current->Token);
        TokenStreamAdvance(tokenStream);
        result = ParseTerm(&rhs, tokenStream);
        if (R_OK != result) {
            return result;
        }
        /* ParseTerm should set us on an operator unless it failed... */
        nextPrec = GetBinaryOperatorPrecedence(tokenStream->Current->Token);
        if (tokenPrec < nextPrec) {
            result = ParseBinaryRhs(&rhs, tokenStream, tokenPrec + 1, rhs);
            if (R_OK != result) {
                return result;
            }
        }
        result = AstMakeBinaryOp(&lhs, lhs, binOp, rhs, tokenStream->Current->Token->SrcLoc);
        if (R_OK != result) {
            return result;
        }
    }
}

/*
 * <binary-expr> := <term> <binary-op> <binary-rhs>
 */
int ParseBinaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Parse right associativity such as 3 ** 3 ** 3 should be equal
     * to 3 ** (3 ** 3) or 3 ** 27 not (3 ** 3) ** 3 */
    int result;
    int opPrec;
    struct Ast *lhs, *ast;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseTerm(&lhs, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if(!IsBinaryOperator(tokenStream->Current->Token)) {
        *out_ast = lhs;
        return R_OK;
    }
    opPrec = GetBinaryOperatorPrecedence(tokenStream->Current->Token);
    result = ParseBinaryRhs(&ast, tokenStream, opPrec, lhs);
    while (1) {
        opPrec = GetBinaryOperatorPrecedence(tokenStream->Current->Token);
        if (-1 == opPrec) {
            break;
        }
        result = ParseBinaryRhs(&ast, tokenStream, opPrec, ast);
    }
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    *out_ast = NULL;
    return result;
}

/*
 * <expr> := <binary-expr>
 *        := <assign>
 *        := <call>
 */
int ParseExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *ast;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseAssign(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseBinaryExpr(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseCall(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseMut(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseConst(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    *out_ast = NULL;
    return R_UnexpectedToken;
}

/*
 * <funciton> := def <identifier> ( <param-list> ) { <stmt-list> }
 *            := def <identifier { <stmt-list> }
 */
int ParseFunction(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result, numArgs, isVarArgs = 0;
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
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
        EXPECT(TokenRightParen, tokenStream); /* ) */
    }
    else {
        params = NULL; /* no params */
    }
    EXPECT(TokenLeftCurlyBrace, tokenStream);  /* { */
    result = ParseStmtList(&body, tokenStream); /* body */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    EXPECT(TokenRightCurlyBrace, tokenStream); /* } */

    if (params) {
        numArgs = params->NumChildren;
    }
    else {
        numArgs = 0;
    }

    FunctionMake(&fn, strdup(funcName), numArgs, isVarArgs, params, body);
    function = malloc(sizeof *function);
    result = ValueMakeFunction(function, fn);
    if (R_OK != result) {
        free(fn);
        free(function);
        AstFree(params);
        AstFree(body);
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    }
    return AstMakeFunction(out_ast, function, save->Token->SrcLoc);
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
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenIf, tokenStream); /* if */

    result = ParseExpr(&cond, tokenStream); /* condition */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);

    EXPECT(TokenLeftCurlyBrace, tokenStream); /* { */

    result = ParseStmtList(&ifBody, tokenStream); /* body of if */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);

    EXPECT(TokenRightCurlyBrace, tokenStream); /* } */
    if (!opt_expect(TokenElse, tokenStream)) { /* no else */
        return AstMakeIfElse(out_ast, cond, ifBody, NULL, save->Token->SrcLoc);
    }
    if (opt_expect(TokenLeftCurlyBrace, tokenStream)) { /* else { */
        result = ParseStmtList(&elseBody, tokenStream);
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
        EXPECT(TokenRightCurlyBrace, tokenStream); /* } */
        return AstMakeIfElse(out_ast, cond, ifBody, elseBody, save->Token->SrcLoc);
    }
    EXPECT(TokenIf, tokenStream); /* else if */
    /* EXPECT eats ths if but we need it to parse a valid IfElse */
    TokenStreamRewind(tokenStream);
    result = ParseIfElse(&elseBody, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeIfElse(out_ast, cond, ifBody, elseBody, save->Token->SrcLoc);
}

/*
 * <stmt> := <expr>
 *        := <function>
 *        := <ifelse>
 *        := <for>
 *        := <while>
 *        := <epsilon>
 */
int ParseStmt(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Node *save;
    struct Ast *ast;
    int result;
    SAVE(tokenStream, save);

    if (check(TokenEOS, tokenStream) || /* End of stream */
        check(TokenSemicolon, tokenStream) || /* ; */
        check(TokenRightCurlyBrace, tokenStream)) { /* { } */
        *out_ast = NULL;
        return R_OK;
    }

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
    result = ParseExpr(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseFor(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseWhile(&ast, tokenStream);
    if (R_OK == result) {
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    return ParseErrorUnexpectedToken(tokenStream->Current->Token);
}

/*
 * <stmt-terminator> := ; TODO: newline stmt-terminator
 *
 * <stmt-list> := <stmt>
 *             := <stmt> <stmt-terminator> <stmt-list>
 */
int ParseStmtList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *tmp;
    struct Ast *ast;
    struct Ast *functionDefs;

    result = AstMakeBlank(&ast);
    if (R_OK != result) {
        out_ast = NULL;
        return result;
    }
    result = AstMakeBlank(&functionDefs);
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
    if (ast->NumChildren == 0) {
        AstFree(ast);
        ast = NULL;
    }
    if (R_OK == result) {
        ast->Type = Body;
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

int ParseTokenStream(struct ParsedTrees *parsedTrees, struct TokenStream *tokenStream) {
    int result;
    struct Node *save;
    struct Ast *functionDefs, *program, *tmp;
    AstMakeBlank(&functionDefs);
    AstMakeBlank(&program);
    SAVE(tokenStream, save);
    while (TokenEOS != tokenStream->Current->Token->Type) {
        result = ParseStmt(&tmp, tokenStream);
        /* Catch any top level function defintions; they need to be stored
           in the symbol table before anything is allowed to execute. */
        if (R_OK == result && tmp && FunctionNode == tmp->Type) {
            result = AstAppendChild(functionDefs, tmp);
        }
        else if (R_OK == result && tmp && FunctionNode != tmp->Type) {
            result = AstAppendChild(program, tmp);
        }
        if (R_OK != result) {
            goto parse_error_cleanup;
        }
    }
    parsedTrees->TopLevelFunctions = functionDefs;
    parsedTrees->Program = program;
    return R_OK;

parse_error_cleanup:
    /* Something happened, restore the old token in case we're in REPL 
       so it can discard the rest of the stream. */ 
    RESTORE(tokenStream, save);
    return result;
}

/************************ Public Functions **************************/


int Parse(struct ParsedTrees *parsedTrees, struct Lexer *lexer) {
    struct TokenStream *tokenStream;
    struct Token *token;
    int result;
    if (!parsedTrees || !lexer) {
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

    result = ParseTokenStream(parsedTrees, tokenStream);
    if (R_OK != result) {
        puts("Parse error!");
        return result;
    }

    return TokenStreamFree(tokenStream);
}
