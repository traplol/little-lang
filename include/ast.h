#ifndef _LITTLE_LANG_AST_H
#define _LITTLE_LANG_AST_H

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

    BooleanNode,
    RealNode,
    IntegerNode,
    StringNode,
    SymbolNode,
    FunctionNode,

    CallExpr,
    ReturnExpr,
    MutExpr,
    ConstExpr,

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
};

void AstPrettyPrint(struct Ast *ast);

int AstMakeBlank(struct Ast **out_ast);
int AstFree(struct Ast *ast);

/* Terminals */
int AstMakeBoolean(struct Ast **out_ast, struct Value *boolean);
int AstMakeReal(struct Ast **out_ast, struct Value *real);
int AstMakeInteger(struct Ast **out_ast, struct Value *integer);
int AstMakeString(struct Ast **out_ast, struct Value *string);
int AstMakeSymbol(struct Ast **out_ast, char *name);
int AstMakeFunction(struct Ast **out_ast, struct Value *function);

/* Expressions should be a combination of more expressions and terminals. */
int AstMakeBinaryOp(struct Ast **out_ast, struct Ast *lhs, enum AstNodeType op, struct Ast *rhs);
int AstMakeUnaryOp(struct Ast **out_ast, enum AstNodeType op, struct Ast *value);
int AstMakeAssign(struct Ast **out_ast, struct Ast *lValue, struct Ast *rhs);
int AstMakeCall(struct Ast **out_ast, char *name, struct Ast *args);
int AstMakeReturn(struct Ast **out_ast, struct Ast *value);
int AstMakeMut(struct Ast **out_ast, struct Ast *names);
int AstMakeConst(struct Ast **out_ast, char *name, struct Ast *value);
int AstMakeFor(struct Ast **out_ast, struct Ast *pre, struct Ast *condition, struct Ast *body, struct Ast *post);
int AstMakeWhile(struct Ast **out_ast, struct Ast *condition, struct Ast *body);
int AstMakeIfElse(struct Ast **out_ast, struct Ast *condition, struct Ast *body, struct Ast *elseif);

int AstAppendChild(struct Ast *ast, struct Ast *child);



#endif
