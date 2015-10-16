#ifndef _LITTLE_LANG_AST_H
#define _LITTLE_LANG_AST_H

#include "src_loc.h"

enum AstNodeType {
    UNASSIGNED,
    Body,

    BAddExpr,
    BSubExpr,
    BMulExpr,
    BDivExpr,
    BModExpr,
    BPowExpr,
    BLShift,
    BRShift,
    BArithOrExpr,
    BArithAndExpr,
    BArithXorExpr,

    BLogicOrExpr,
    BLogicAndExpr,
    BLogicEqExpr,
    BLogicNotEqExpr,
    BLogicLtExpr,
    BLogicLtEqExpr,
    BLogicGtExpr,
    BLogicGtEqExpr,

    UNegExpr,
    ULogicNotExpr,

    AssignExpr,

    NilNode,
    BooleanNode,
    RealNode,
    IntegerNode,
    StringNode,
    SymbolNode,
    FunctionNode,
    ClassNode,

    CallExpr,
    ArrayIdxExpr,
    MemberAccessExpr,

    ReturnExpr,
    ContinueExpr,
    BreakExpr,
    MutExpr,
    ConstExpr,

    ImportExpr,

    ForExpr,
    WhileExpr,
    IfElseExpr,
};

struct Ast {
    enum AstNodeType Type;
    struct Ast **Children;
    unsigned int NumChildren;
    unsigned int CapChildren;
    union {
        char *SymbolName;
        struct Value *Value;
    } u;
    struct SrcLoc SrcLoc;
};

void AstPrettyPrint(struct Ast *ast);

int AstMakeBlank(struct Ast **out_ast);
int AstFree(struct Ast *ast);

int AstDeepCopy(struct Ast **out_ast, struct Ast *ast);

/* Terminals */
int AstMakeBoolean(struct Ast **out_ast, struct Value *boolean, struct SrcLoc srcLoc);
int AstMakeTrue(struct Ast **out_ast, struct SrcLoc srcLoc);
int AstMakeFalse(struct Ast **out_ast, struct SrcLoc srcLoc);
int AstMakeNil(struct Ast **out_ast, struct SrcLoc srcLoc);
int AstMakeReal(struct Ast **out_ast, struct Value *real, struct SrcLoc srcLoc);
int AstMakeInteger(struct Ast **out_ast, struct Value *integer, struct SrcLoc srcLoc);
int AstMakeString(struct Ast **out_ast, struct Value *string, struct SrcLoc srcLoc);
int AstMakeSymbol(struct Ast **out_ast, char *name, struct SrcLoc srcLoc);
int AstMakeFunction(struct Ast **out_ast, struct Value *function, struct SrcLoc srcLoc);
int AstMakeImport(struct Ast **out_ast, struct Ast *modName, struct Ast *as, struct SrcLoc srcLoc);
int AstMakeClass(struct Ast **out_ast, char *className, struct Ast *body, struct SrcLoc srcLoc);

/* Expressions should be a combination of more expressions and terminals. */
int AstMakeBinaryOp(struct Ast **out_ast, struct Ast *lhs, enum AstNodeType op, struct Ast *rhs, struct SrcLoc srcLoc);
int AstMakeUnaryOp(struct Ast **out_ast, enum AstNodeType op, struct Ast *value, struct SrcLoc srcLoc);
int AstMakeAssign(struct Ast **out_ast, struct Ast *lValue, struct Ast *rhs, struct SrcLoc srcLoc);
int AstMakeCall(struct Ast **out_ast, struct Ast *primary, struct Ast *args, struct SrcLoc srcLoc);
int AstMakeArrayIdx(struct Ast **out_ast, struct Ast *postfix, struct Ast *expr, struct SrcLoc srcLoc);
int AstMakeMemberAccess(struct Ast **out_ast, struct Ast *postfix, struct Ast *symbol, struct SrcLoc srcLoc);
int AstMakeReturn(struct Ast **out_ast, struct Ast *expr, struct SrcLoc srcLoc);
int AstMakeContinue(struct Ast **out_ast, struct SrcLoc srcLoc);
int AstMakeBreak(struct Ast **out_ast, struct SrcLoc srcLoc);
int AstMakeMut(struct Ast **out_ast, struct Ast *names, struct Ast *values, struct SrcLoc srcLoc);
int AstMakeConst(struct Ast **out_ast, char *name, struct Ast *value, struct SrcLoc srcLoc);
int AstMakeFor(struct Ast **out_ast, struct Ast *pre, struct Ast *condition, struct Ast *body, struct Ast *post, struct SrcLoc srcLoc);
int AstMakeWhile(struct Ast **out_ast, struct Ast *condition, struct Ast *body, struct SrcLoc srcLoc);
int AstMakeIfElse(struct Ast **out_ast, struct Ast *condition, struct Ast *body, struct Ast *elseif, struct SrcLoc srcLoc);

int AstAppendChild(struct Ast *ast, struct Ast *child);



#endif
