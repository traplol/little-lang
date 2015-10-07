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
    [TokenNewline] = "<newline>",
    [TokenComma] = ",",
    [TokenIdentifer] = "<ident>",
    [TokenStringLiteral] = "<string>",
    [TokenDef] = "def",
    [TokenMut] = "mut",
    [TokenConst] = "const",
    [TokenFor] = "for",
    [TokenWhile] = "while",
    [TokenIf] = "if",
    [TokenAs] = "as",
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
            return ParseErrorUnexpectedTokenExpected(typ, (ts)->Current->Token); \
        }                                                               \
        else {                                                          \
            TokenStreamAdvance((ts));                                   \
        }                                                               \
    } while(0)

#define EXPECT_EITHER(typ1, typ2, ts)                                   \
    do {                                                                \
        if ((typ1) != (ts)->Current->Token->Type && (typ2) != (ts->Current->Token->Type)) { \
            return ParseErrorUnexpectedTokenExpectedEither(typ1, typ2, (ts)->Current->Token); \
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

int ParseErrorUnexpectedTokenExpected(enum TokenType type, struct Token *token) {
    fprintf(stderr, "Unexpected token: '%s' at %s:%d:%d, expected '%s'\n",
            token->TokenStr,
            token->SrcLoc.Filename,
            token->SrcLoc.LineNumber,
            token->SrcLoc.ColumnNumber,
            tokenStrings[type]);
    return R_UnexpectedToken;
}

int ParseErrorUnexpectedTokenExpectedEither(enum TokenType type1, enum TokenType type2, struct Token *token) {
    fprintf(stderr, "Unexpected token: '%s' at %s:%d:%d, expected either '%s' or '%s'\n",
            token->TokenStr,
            token->SrcLoc.Filename,
            token->SrcLoc.LineNumber,
            token->SrcLoc.ColumnNumber,
            tokenStrings[type1],
            tokenStrings[type2]);
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
int ParseExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseAssign(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseIfElse(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseFunction(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseWhile(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseFor(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseDeclStmt(struct Ast **out_ast, struct TokenStream *tokenStream);
//int ParseStmt(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseStmtList(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseLiteral(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseIdentifier(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseParenExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParsePrimary(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseBinaryRhs(struct Ast **out_ast, struct TokenStream *tokenStream, int prec, struct Ast *lhs);
int ParseBinaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
int ParseUnaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream);
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
    result = ParseAssign(&expr, tokenStream);
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
    while(R_OK == (result = ParseAssign(&curExpr, tokenStream))) {
        AstAppendChild(exprs, curExpr);
        if (!opt_expect(TokenComma, tokenStream)) {
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

/* <mut-expr> := mut <comma-separated-identifiers> = <comma-separated-exprs>
 *            := mut <comma-separated-identifiers>
 */
int ParseMut(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Validate 'mut-expr' */
    int result;
    struct Ast *names, *exprs;
    struct Node *save;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenMut, tokenStream);
    result = ParseCommaSeparatedIdentifiers(&names, tokenStream);
    if (check(TokenEquals, tokenStream)) {
        EXPECT(TokenEquals, tokenStream);
        result = ParseCommaSeparatedExprs(&exprs, tokenStream);
        return AstMakeMut(out_ast, names, exprs, save->Token->SrcLoc);
    }
    return AstMakeMut(out_ast, names, NULL, save->Token->SrcLoc);
}

/* <for> := for <expr> ; <expr> ; <expr> { <stmt-list> } */
int ParseFor(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Validate 'for' parsing. */
    int result;
    struct Ast *pre, *cond, *post, *body;
    struct Node *save;
    SAVE(tokenStream, save);

    EXPECT_NO_MSG(TokenFor, tokenStream);

    result = ParseAssign(&pre, tokenStream);
    if (R_OK != result) {
        result = ParseDeclStmt(&pre, tokenStream);
    }
    EXPECT(TokenSemicolon, tokenStream);

    result = ParseAssign(&cond, tokenStream);
    EXPECT(TokenSemicolon, tokenStream);

    result = ParseAssign(&post, tokenStream);

    result = ParseStmtList(&body, tokenStream);

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

    result = ParseAssign(&cond, tokenStream);

    result = ParseStmtList(&body, tokenStream);

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
 * <arg-list> := <assign>
 *            := <assign> , <arg-list>
 *            := <epsilon>
 */
int ParseArgList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *args, *arg;
    result = AstMakeBlank(&args); /* FIXME: Do something with result. */
    while (R_OK == (result = ParseAssign(&arg, tokenStream))) {
        AstAppendChild(args, arg);
        if (!opt_expect(TokenComma, tokenStream)) {
            break;
        }
    }
    *out_ast = args;
    return result;
}
/* ( arg-list ), [ expr ], .identifier, etc */
int ParsePostfixRhs(struct Ast **out_ast, struct TokenStream *tokenStream, struct Ast *expr) {
    int result;
    union {
        struct Ast *arglist;
        struct Ast *expr;
        struct Ast *identifer;
    } u;
    struct Node *save;
    SAVE(tokenStream, save);
    if (opt_expect(TokenLeftParen, tokenStream)) {
        if (opt_expect(TokenRightParen, tokenStream)) { /* No arguments. */
            return AstMakeCall(out_ast, expr, NULL, save->Token->SrcLoc);
        }
        result = ParseArgList(&u.arglist, tokenStream);
        EXPECT(TokenRightParen, tokenStream);
        if (R_OK == result) {
            return AstMakeCall(out_ast, expr, u.arglist, save->Token->SrcLoc);
        }
    }
    else if(opt_expect(TokenLeftSqBracket, tokenStream)) {
        result = ParseAssign(&u.expr, tokenStream);
        EXPECT(TokenRightSqBracket, tokenStream);
        if (R_OK == result) {
            return AstMakeArrayIdx(out_ast, expr, u.expr, save->Token->SrcLoc);
        }
    }
    else if (opt_expect(TokenDot, tokenStream)) {
        EXPECT(TokenIdentifer, tokenStream); /* Expect here because we know an identifier should follow. */
        TokenStreamRewind(tokenStream);
        result = ParseIdentifier(&u.identifer, tokenStream);
        if (R_OK == result) {
            return AstMakeMemberAccess(out_ast, expr, u.identifer, save->Token->SrcLoc);
        }
    }
    *out_ast = NULL;
    return R_UnexpectedToken;
}

/*
 * <postfix-expr> := <primary> 
 *                := <postfix-expr> ( <arg-list> )            // call
 *                := <postfix-expr> [ <expr> ]                // array idx
 *                := <postfix-expr> . <identifer>             // member access
 */
int ParsePostfix(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *expr, *tmp;
    struct Node *save;
    result = ParsePrimary(&expr, tokenStream);
    if (R_OK != result) {
        *out_ast = NULL;
        return R_UnexpectedToken;
    }

    SAVE(tokenStream, save);
    result = ParsePostfixRhs(&tmp, tokenStream, expr);
    if (R_OK != result) {
        RESTORE(tokenStream, save);
        *out_ast = expr;
        return R_OK;
    }

    while (check(TokenLeftParen, tokenStream) ||
           check(TokenLeftSqBracket, tokenStream) ||
           check(TokenDot, tokenStream)) { /* Parse the right side of the postfix expr */
        result = ParsePostfixRhs(&tmp, tokenStream, tmp);
    }
    *out_ast = tmp;
    return R_OK;
}
/*
 * <literal>
 */
int ParseLiteral(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Node *save;
    struct Value *value;
    struct Token *token = tokenStream->Current->Token;
    SAVE(tokenStream, save);
    switch (token->Type) {
        default:
            goto fail_cleanup;
        case TokenIntegerConstant:
            if (R_OK != ValueMakeIntegerLiteral(&value, token->v.Integer)) {
                goto fail_cleanup;
            }
            result = AstMakeInteger(out_ast, value, save->Token->SrcLoc);
            goto success;
        case TokenRealConstant:
            if (R_OK != ValueMakeRealLiteral(&value, token->v.Real)) {
                goto fail_cleanup;
            }
            result = AstMakeReal(out_ast, value, save->Token->SrcLoc);
            goto success;
        case TokenStringLiteral:
            if (R_OK != ValueMakeLLStringLiteral(&value, token->v.String)) {
                goto fail_cleanup;
            }
            result = AstMakeString(out_ast, value, save->Token->SrcLoc);
            goto success;
        case TokenTrue:
            result = AstMakeTrue(out_ast, save->Token->SrcLoc);
            goto success;
        case TokenFalse:
            result = AstMakeFalse(out_ast, save->Token->SrcLoc);
            goto success;
        case TokenNil:
            result = AstMakeNil(out_ast, save->Token->SrcLoc);
            goto success;
    }

success:
    TokenStreamAdvance(tokenStream);
    return result;

fail_cleanup:
    RESTORE(tokenStream, save);
    return R_UnexpectedToken;
}

/*
 * <identifier>
 */
int ParseIdentifier(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Node *save;
    char *identifier = tokenStream->Current->Token->TokenStr;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenIdentifer, tokenStream);
    return AstMakeSymbol(out_ast, identifier, save->Token->SrcLoc);
}

/*
 * <paren-expr> := ( <expr> )
 */
int ParseParenExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *expr;
    EXPECT_NO_MSG(TokenLeftParen, tokenStream);
    result = ParseAssign(&expr, tokenStream);
    EXPECT(TokenRightParen, tokenStream);
    *out_ast = expr;
    return result;
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
        result = ParseUnaryExpr(&rhs, tokenStream);
        if (R_OK != result) {
            return result;
        }
        /* ParsePrimary should set us on an operator unless it failed... */
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

/* <primary-expr> := <identifier>
 *                := <literal> 
 *                := <paren-expr>
 */
int ParsePrimary(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *primary;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseIdentifier(&primary, tokenStream);
    if (R_OK == result) {
        *out_ast = primary;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseLiteral(&primary, tokenStream);
    if (R_OK == result) {
        *out_ast = primary;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseParenExpr(&primary, tokenStream);
    if (R_OK == result) {
        *out_ast = primary;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    *out_ast = NULL;
    return R_UnexpectedToken;
}

/* 
 * <unary-expr> := <postfix-expr>    
 *              := <unary-op> <unary-expr> 
 */
int ParseUnaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    enum AstNodeType unOp;
    struct Ast *postfix, *expr;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParsePostfix(&postfix, tokenStream);
    if (R_OK == result) {
        *out_ast = postfix;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    if (!IsUnaryOperator(tokenStream->Current->Token)) {
        return R_UnexpectedToken;
    }
    unOp = GetUnaryOperatorType(tokenStream->Current->Token);
    TokenStreamAdvance(tokenStream);
    result = ParseUnaryExpr(&expr, tokenStream);
    if (R_OK == result) {
        return AstMakeUnaryOp(out_ast, unOp, expr, save->Token->SrcLoc);
    }
    *out_ast = NULL;
    return result;
}


/*
 * <binary-expr> := <unary-expr> <binary-op> <binary-rhs> 
 */
int ParseBinaryExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    /* TODO: Parse right associativity such as 3 ** 3 ** 3 should be equal
     * to 3 ** (3 ** 3) or 3 ** 27 not (3 ** 3) ** 3 */
    int result;
    int opPrec;
    struct Ast *lhs, *ast;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseUnaryExpr(&lhs, tokenStream);
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
 * <assign> := <binary-expr>
 *          := <unary-expr> <assignment-operator> <assign> 
 */
int ParseAssign(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *unary, *expr;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseBinaryExpr(&expr, tokenStream);
    if (R_OK == result && !check(TokenEquals, tokenStream)) {
        *out_ast = expr;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseUnaryExpr(&unary, tokenStream);
    if (R_OK != result) {
        *out_ast = NULL;
        return result;
    }
    EXPECT_NO_MSG(TokenEquals, tokenStream);
    result = ParseAssign(&expr, tokenStream);
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
    return AstMakeAssign(out_ast, unary, expr, save->Token->SrcLoc);
}

/*
 * <expr> := <assign> 
 */
int ParseExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *ast;
    struct Node *save;
    SAVE(tokenStream, save);
    result = ParseAssign(&ast, tokenStream);
    if (R_OK == result) {
        OPT_EXPECT(TokenSemicolon, tokenStream);
        *out_ast = ast;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    result = ParseDeclStmt(&ast, tokenStream);
    if (R_OK == result) {
        OPT_EXPECT(TokenSemicolon, tokenStream);
        *out_ast = ast;
        return R_OK;
    }
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
    result = ParseStmtList(&body, tokenStream); /* body */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);

    if (params) {
        numArgs = params->NumChildren;
    }
    else {
        numArgs = 0;
    }

    FunctionMake(&fn, funcName, numArgs, isVarArgs, params, body);
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
    struct Node *save, *beforeElse;
    struct Ast *cond, *ifBody, *elseBody;
    cond = ifBody = elseBody = NULL;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenIf, tokenStream); /* if */

    result = ParseAssign(&cond, tokenStream); /* condition */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);


    result = ParseStmtList(&ifBody, tokenStream); /* body of if */
    IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);

    SAVE(tokenStream, beforeElse);
    while (opt_expect(TokenNewline, tokenStream)) {
        ;
    }
    if (!opt_expect(TokenElse, tokenStream)) { /* no else */
        RESTORE(tokenStream, beforeElse);
        return AstMakeIfElse(out_ast, cond, ifBody, NULL, save->Token->SrcLoc);
    }
    if (check(TokenLeftCurlyBrace, tokenStream)) { /* else { */
        result = ParseStmtList(&elseBody, tokenStream);
        IF_FAIL_RETURN_PARSE_ERROR(result, tokenStream, save, out_ast);
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
 * <decl-stmt> := <mut>
 *             := <const>
 */
int ParseDeclStmt(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result = 0;
    struct Ast *ast;
    struct Node *save;
    SAVE(tokenStream, save);
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
    *out_ast = NULL;
    return R_UnexpectedToken;
}

/*
 * <stmt> := <expr>
 *        := <function>
 *        := <ifelse>
 *        := <for>
 *        := <while>
 *        := <decl-stmt>
 */
int ParseStmt(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Node *save;
    struct Ast *ast;
    int result;
    SAVE(tokenStream, save);

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
    if (check(TokenEOS, tokenStream) || /* End of stream */
        check(TokenSemicolon, tokenStream) || /* ; */
        check(TokenNewline, tokenStream) || /* <newline> */
        check(TokenRightCurlyBrace, tokenStream)) { /* { */
        *out_ast = NULL;
        return R_OK;
    }
    RESTORE(tokenStream, save);
    return R_UnexpectedToken;
}

/*
 * <stmt-terminator> := ; <statement-terminator>
 *                   := <newline> <statement-terminator>
 *
 * <stmt-list> := { <stmt> }
 *             := { <stmt> <stmt-terminator> <stmt-list> }
 */
int ParseStmtList(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *tmp;
    struct Ast *ast;
    while (opt_expect(TokenNewline, tokenStream)) {
        ;
    }
    EXPECT(TokenLeftCurlyBrace, tokenStream);
    result = AstMakeBlank(&ast);
    if (R_OK != result) {
        out_ast = NULL;
        return result;
    }
    while (R_OK == (result = ParseStmt(&tmp, tokenStream))) {
        AstAppendChild(ast, tmp);
        if (check(TokenRightCurlyBrace, tokenStream)) {
            break;
        }
        if (check(TokenEOS, tokenStream)) {
            break;
        }
        EXPECT_EITHER(TokenSemicolon, TokenNewline, tokenStream);
    }
    EXPECT(TokenRightCurlyBrace, tokenStream);
    if (R_OK == result) {
        ast->Type = Body;
        *out_ast = ast;
    }
    else {
        *out_ast = NULL;
    }
    return result;
}

/* <import> := import <string-literal> as <identifier> */
int ParseImport(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *modName, *as = NULL;
    struct Node *save;
    SAVE(tokenStream, save);
    EXPECT_NO_MSG(TokenImport, tokenStream);
    EXPECT(TokenStringLiteral, tokenStream);
    TokenStreamRewind(tokenStream);
    result = ParseLiteral(&modName, tokenStream);
    EXPECT(TokenAs, tokenStream);
    EXPECT(TokenIdentifer, tokenStream);
    TokenStreamRewind(tokenStream);
    result = ParseIdentifier(&as, tokenStream);
    return AstMakeImport(out_ast, modName, as, save->Token->SrcLoc);
}

/* <class-expr> := <mut-expr>
 *              := <const-expr>
 *              := <function>
 */
int ParseClassExpr(struct Ast **out_ast, struct TokenStream *tokenStream) {
    
}

/* <class-body> := <class-expr>
 *              := <class-expr> <class-body>
 */
int ParseClassBody(struct Ast **out_ast, struct TokenStream *tokenStream) {
    
}

/* <class> := class <identifier> { <class-body> } */
int ParseClass(struct Ast **out_ast, struct TokenStream *tokenStream) {
    struct Ast *class;
    EXPECT_NO_MSG(TokenClass, tokenStream);
}

/*
 * <program> := <stmt-list>
 *           := <import>
 */
int ParseTokenStream(struct ParsedTrees *parsedTrees, struct TokenStream *tokenStream) {
    int result;
    struct Node *save, *tryImport;
    struct Ast *imports, *classes, *functionDefs, *program, *tmp;
    AstMakeBlank(&imports);
    AstMakeBlank(&classes);
    AstMakeBlank(&functionDefs);
    AstMakeBlank(&program);
    SAVE(tokenStream, save);
    while (tokenStream->Current && TokenEOS != tokenStream->Current->Token->Type) {
        SAVE(tokenStream, tryImport);
        result = ParseImport(&tmp, tokenStream);
        if (R_OK == result) {
            AstAppendChild(imports, tmp);
            continue;
        }
        RESTORE(tokenStream, tryImport);
        //result = ParseClass(&tmp, tokenStream);
        //if (R_OK == result) {
        //AstAppendChild(classes, tmp);
        //continue;
        //}
        //RESTORE(tokenStream, tryImport);
        result = ParseStmt(&tmp, tokenStream);
        if (!check(TokenEOS, tokenStream)) {
            EXPECT_EITHER(TokenSemicolon, TokenNewline, tokenStream);
        }
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
    parsedTrees->Imports = imports;
    parsedTrees->Classes = classes;
    parsedTrees->TopLevelFunctions = functionDefs;
    parsedTrees->Program = program;
    return R_OK;

parse_error_cleanup:
    return result;
}

/************************ Public Functions **************************/

int ParseThing(struct Ast **out_ast, struct TokenStream *tokenStream) {
    int result;
    struct Ast *out;
    result = ParseImport(&out, tokenStream);
    if (R_OK == result) {
        *out_ast = out;
        return R_OK;
    }
    return ParseStmt(out_ast, tokenStream);
}

int Parse(struct ParsedTrees *parsedTrees, struct Lexer *lexer) {
    struct TokenStream *tokenStream;
    int result;
    if (!parsedTrees || !lexer) {
        return R_InvalidArgument;
    }
    tokenStream = malloc(sizeof *tokenStream);
    result = TokenStreamMake(tokenStream, lexer);
    if (R_OK != result) {
        return result;
    }

    result = ParseTokenStream(parsedTrees, tokenStream);
    if (R_OK != result) {
        puts("Parse error!");
        TokenStreamFree(tokenStream);
        return result;
    }
    result = TokenStreamFree(tokenStream);
    free(tokenStream);
    return result;
}
