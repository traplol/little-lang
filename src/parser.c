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
    result = AstMakeBlank(&params); /* FIXME: Do something with result. */
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
    EXPECT_NO_MSG(TokenIdentifer, tokenStream);
    EXPECT(TokenLeftParen, tokenStream);
    result = ParseArgList(&args, tokenStream); /* FIXME: Do something with result. */
    EXPECT(TokenRightParen, tokenStream);
    identifier = strdup(identifier);
    return AstMakeCall(out_ast, identifier, args);
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
    return AstMakeAssign(out_ast, lvalue, expr);
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
    EXPECT_NO_MSG(TokenIdentifer, tokenStream);
    identifier = strdup(identifier);
    return AstMakeSymbol(out_ast, identifier);
}

/* TOP PRECEDENCE
 * <prec1> := <literal>
 *         := ( <expr> )
 */
int ParsePrec1(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Token *token = tokenStream->Current->Token;
    struct Ast *prec1;
    switch (token->Type) {
        default:
            break;
        case TokenIntegerConstant:
        case TokenRealConstant:
        case TokenStringLiteral:
        case TokenTrue:
        case TokenFalse:
            return ParseLiteral(out_ast, tokenStream); /* <literal> */
    }
    if (opt_expect(TokenLeftParen, tokenStream)) { /* ( <expr> ) */
        result = ParseExpr(&prec1, tokenStream);
        EXPECT(TokenRightParen, tokenStream);
        *out_ast = prec1;
        return result;
    }
    return R_UnexpectedToken;
}

/*
 * <prec2> := <prec1>
 *         := ! <prec1>
 *         := - <prec1>
 */
int ParsePrec2(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec1;
    struct Node *save;
    SAVE(tokenStream, save);
    // FIXME: need to check for prefix ops first. 
    if (opt_expect(TokenBang, tokenStream)) { /* ! */
        op = ULogicNotExpr;
    }
    else if (opt_expect(TokenMinus, tokenStream)) { /* - */
        op = UNegExpr;
    }
    else {
        result = ParsePrec1(&prec1, tokenStream);
        *out_ast = R_OK == result ? prec1 : NULL;
        return result;
    }
    result = ParsePrec1(&prec1, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeUnaryOp(out_ast, op, prec1);
}

/*
 * <prec3> := <prec2>
 *         := <prec2> ** <prec3>
 */
int ParsePrec3(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec2, *prec3;
    struct Node *save;
    SAVE(tokenStream, save);
    
    /* FIXME: Pow is right associative. */
    result = ParsePrec2(&prec2, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenStarStar, tokenStream)) { /* ** */
        op = BPowExpr;
    }
    else {
        *out_ast = prec2;
        return R_OK;
    }
    result = ParsePrec3(&prec3, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec2, op, prec3);
}
/*
 * <prec4> := <prec3>
 *         := <prec3> * <prec4>
 *         := <prec3> / <prec4>
 *         := <prec3> % <prec4>
 */
int ParsePrec4(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec3, *prec4;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec3(&prec3, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenAsterisk, tokenStream)) { /* * */
        op = BMulExpr;
    }
    else if (opt_expect(TokenSlash, tokenStream)) { /* / */
        op = BDivExpr;
    }
    else if (opt_expect(TokenPercent, tokenStream)) { /* % */
        op = BModExpr;
    }
    else {
        *out_ast = prec3;
        return R_OK;
    }
    result = ParsePrec4(&prec4, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec3, op, prec4);
}

/*
 * <prec5> := <prec4>
 *         := <prec4> + <prec5>
 *         := <prec4> - <prec5>
 */
int ParsePrec5(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec4, *prec5;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec4(&prec4, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenPlus, tokenStream)) { /* + */
        op = BAddExpr;
    }
    else if (opt_expect(TokenMinus, tokenStream)) { /* - */
        op = BSubExpr;
    }
    else {
        *out_ast = prec4;
        return R_OK;
    }
    result = ParsePrec5(&prec5, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec4, op, prec5);
}

/*
 * <prec6> := <prec5>
 *         := <prec5> << <prec6>
 *         := <prec5> >> <prec6>
 */
int ParsePrec6(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec5, *prec6;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec5(&prec5, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenLtLt, tokenStream)) { /* << */
        op = BLShift;
    }
    else if (opt_expect(TokenGtGt, tokenStream)) { /* >> */
        op = BRShift;
    }
    else {
        *out_ast = prec5;
        return R_OK;
    }
    result = ParsePrec6(&prec6, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec5, op, prec6);
}

/*
 * <prec7> := <prec6>
 *         := <prec6> & <prec7>
 */
int ParsePrec7(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec6, *prec7;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec6(&prec6, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenAmp, tokenStream)) { /* & */
        op = BArithAndExpr;
    }
    else {
        *out_ast = prec6;
        return R_OK;
    }
    result = ParsePrec7(&prec7, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec6, op, prec7);
}

/*
 * <prec8> := <prec7>
 *         := <prec7> ^ <prec6>
 */
int ParsePrec8(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec7, *prec8;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec7(&prec7, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenCaret, tokenStream)) { /* ^ */
        op = BArithXorExpr;
    }
    else {
        *out_ast = prec7;
        return R_OK;
    }
    result = ParsePrec8(&prec8, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec7, op, prec8);
}

/*
 * <prec9> := <prec8>
 *         := <prec8> | <prec9>
 */
int ParsePrec9(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec8, *prec9;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec8(&prec8, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenBar, tokenStream)) { /* | */
        op = BArithOrExpr;
    }
    else {
        *out_ast = prec8;
        return R_OK;
    }
    result = ParsePrec9(&prec9, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec8, op, prec9);
}

/*
 * <prec10> := <prec9>
 *          := <prec9> > <prec10>
 *          := <prec9> >= <prec10>
 *          := <prec9> < <prec10>
 *          := <prec9> <= <prec10>
 */
int ParsePrec10(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec9, *prec10;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec9(&prec9, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenLt, tokenStream)) { /* < */
        op = BLogicLtExpr;
    }
    else if (opt_expect(TokenLtEq, tokenStream)) { /* <= */
        op = BLogicLtEqExpr;
    }
    else if (opt_expect(TokenGt, tokenStream)) { /* > */
        op = BLogicGtExpr;
    }
    else if (opt_expect(TokenGtEq, tokenStream)) { /* >= */
        op = BLogicGtEqExpr;
    }
    else {
        *out_ast = prec9;
        return R_OK;
    }
    result = ParsePrec10(&prec10, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec9, op, prec10);
}

/*
 * <prec11> := <prec10>
 *          := <prec10> == <prec11>
 *          := <prec10> != <prec11>
 */
int ParsePrec11(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec10, *prec11;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec10(&prec10, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenEqEq, tokenStream)) { /* == */
        op = BLogicEqExpr;
    }
    else if (opt_expect(TokenBangEq, tokenStream)) { /* != */
        op = BLogicNotEqExpr;
    }
    else {
        *out_ast = prec10;
        return R_OK;
    }
    result = ParsePrec11(&prec11, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec10, op, prec11);
}

/*
 * <prec12> := <prec11>
 *          := <prec11> && <prec12>
 */
int ParsePrec12(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec11, *prec12;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec11(&prec11, tokenStream);
    if (R_OK != result) {
        return result;
    }

    if (opt_expect(TokenAmpAmp, tokenStream)) { /* && */
        op = BLogicAndExpr;
    }
    else {
        *out_ast = prec11;
        return R_OK;
    }
    result = ParsePrec12(&prec12, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec11, op, prec12);
}

/*
 * <prec13> := <prec12>
 *          := <prec12> || <prec13>
 */

int ParsePrec13(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType op = UNASSIGNED;
    struct Ast *prec12, *prec13;
    struct Node *save;
    SAVE(tokenStream, save);
    
    result = ParsePrec12(&prec12, tokenStream);
    if (R_OK != result) {
        return result;
    }
    if (opt_expect(TokenBarBar, tokenStream)) { /* || */
        op = BLogicOrExpr;
    }
    else {
        *out_ast = prec12;
        return R_OK;
    }
    result = ParsePrec13(&prec13, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeBinaryOp(out_ast, prec12, op, prec13);
}

/*
 * <expr> := <prec13>
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
    result = ParsePrec13(&ast, tokenStream);
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
    *out_ast = NULL;
    return R_UnexpectedToken;
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
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
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
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);

    EXPECT(TokenLeftCurlyBrace, tokenStream); /* { */

    result = ParseStmtList(&ifBody, tokenStream); /* body of if */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);

    EXPECT(TokenRightCurlyBrace, tokenStream); /* } */
    if (!opt_expect(TokenElse, tokenStream)) { /* no else */
        return AstMakeIfElse(out_ast, cond, ifBody, NULL);
    }
    if (opt_expect(TokenLeftCurlyBrace, tokenStream)) { /* else { */
        result = ParseStmtList(&elseBody, tokenStream);
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
        EXPECT(TokenRightCurlyBrace, tokenStream); /* } */
        return AstMakeIfElse(out_ast, cond, ifBody, elseBody);
    }
    EXPECT(TokenIf, tokenStream); /* else if */
    /* EXPECT eats ths if but we need it to parse a valid IfElse */
    TokenStreamRewind(tokenStream);
    result = ParseIfElse(&elseBody, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeIfElse(out_ast, cond, ifBody, elseBody);
}

/*
 * <stmt> := <expr>
 *        := <function>
 *        := <ifelse>
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
    if (ast->NumChildren == 0) {
        /* TODO: Free ast. */
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

int ParseTokenStream(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Node *save;
    SAVE(tokenStream, save);
    while (TokenEOS != tokenStream->Current->Token->Type) {
        result = ParseStmtList(out_ast, tokenStream);
        if (R_OK != result) {
            goto parse_error_cleanup;
        }
    }
    return R_OK;

parse_error_cleanup:
    /* Something happened, restore the old token in case we're in REPL 
       so it can discard the rest of the stream. */ 
    RESTORE(tokenStream, save);
    return result;
}

/****************** Simple eval to test arithmetic ops ******************/

struct Ast *left(struct Ast *ast) {
    if (!ast->Children) return NULL;
    return ast->Children[0];
}
struct Ast *right(struct Ast *ast) {
    if (!ast->Children) return NULL;
    return ast->Children[1];
}
int value(struct Value *value) {
    return value->v.Integer;
}
struct Value *eval(struct Ast *ast);

struct Value *add(struct Ast *lhs, struct Ast *rhs) {
    struct Value *lResult = eval(lhs);
    struct Value *rResult = eval(rhs);
    int r = value(lResult) + value(rResult);
    struct Value *result = malloc(sizeof *result);
    ValueMake(result, &g_TheIntegerTypeInfo, &r, sizeof r);
    return result;
}
struct Value *sub(struct Ast *lhs, struct Ast *rhs) {
    struct Value *lResult = eval(lhs);
    struct Value *rResult = eval(rhs);
    int r = value(lResult) - value(rResult);
    struct Value *result = malloc(sizeof *result);
    ValueMake(result, &g_TheIntegerTypeInfo, &r, sizeof r);
    return result;
}
struct Value *mul(struct Ast *lhs, struct Ast *rhs) {
    struct Value *lResult = eval(lhs);
    struct Value *rResult = eval(rhs);
    int r = value(lResult) * value(rResult);
    struct Value *result = malloc(sizeof *result);
    ValueMake(result, &g_TheIntegerTypeInfo, &r, sizeof r);
    return result;
}
struct Value *divide(struct Ast *lhs, struct Ast *rhs) {
    struct Value *lResult = eval(lhs);
    struct Value *rResult = eval(rhs);
    int r = value(lResult) / value(rResult);
    struct Value *result = malloc(sizeof *result);
    ValueMake(result, &g_TheIntegerTypeInfo, &r, sizeof r);
    return result;
}
struct Value *mod(struct Ast *lhs, struct Ast *rhs) {
    struct Value *lResult = eval(lhs);
    struct Value *rResult = eval(rhs);
    int r = value(lResult) % value(rResult);
    struct Value *result = malloc(sizeof *result);
    ValueMake(result, &g_TheIntegerTypeInfo, &r, sizeof r);
    return result;
}
struct Value *eval(struct Ast *ast) {
    struct Ast *lhs, *rhs;
    lhs = left(ast);
    rhs = right(ast);
    switch (ast->Type) {
        default:
            printf("<UNEXPECTED>\n");
            return 0;
        case IntegerNode:
            return ast->u.Value;
        case BAddExpr:
            return add(lhs, rhs);
        case BSubExpr:
            return sub(lhs, rhs);
        case BMulExpr:
            return mul(lhs, rhs);
        case BDivExpr:
            return divide(lhs, rhs);
        case BModExpr:
            return mod(lhs, rhs);
    }
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
//    printf("eval: %d\n", value(eval((*out_ast)->Children[0])));
    puts("Parse succesfull!");

    return TokenStreamFree(tokenStream);
}
