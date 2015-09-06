#include "ast.h"
#include "result.h"

#include <stdlib.h>

int AstFree(struct Ast *ast) {
    unsigned int i;
    if (!ast) {
        return R_InvalidArgument;
    }
    for (i = 0; i < ast->NumChildren; ++i) {
        AstFree(ast->Children[i]);
    }
    if (ast->NumChildren > 0) {
        free(ast->Children);
    }
    free(ast);
    return R_OK;
}

int AstMakeBoolean(struct Ast **out_ast, struct Value *boolean) {
    struct Ast *ast;
    if (!out_ast || !boolean) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = BooleanNode;
    ast->Children = NULL;
    ast->NumChildren = 0;
    ast->u.Value = boolean;
    *out_ast = ast;
    return R_OK;
}
int AstMakeReal(struct Ast **out_ast, struct Value *real) {
    struct Ast *ast;
    if (!out_ast || !real) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = RealNode;
    ast->Children = NULL;
    ast->NumChildren = 0;
    ast->u.Value = real;
    *out_ast = ast;
    return R_OK;
}
int AstMakeInteger(struct Ast **out_ast, struct Value *integer) {
    struct Ast *ast;
    if (!out_ast || !integer) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = IntegerNode;
    ast->Children = NULL;
    ast->NumChildren = 0;
    ast->u.Value = integer;
    *out_ast = ast;
    return R_OK;
}
int AstMakeString(struct Ast **out_ast, struct Value *string) {
    struct Ast *ast;
    if (!out_ast || !string) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = StringNode;
    ast->Children = NULL;
    ast->NumChildren = 0;
    ast->u.Value = string;
    *out_ast = ast;
    return R_OK;
}
int AstMakeSymbol(struct Ast **out_ast, char *name) {
    struct Ast *ast;
    if (!out_ast || !name) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = SymbolNode;
    ast->Children = NULL;
    ast->NumChildren = 0;
    ast->u.SymbolName = name; /* FIXME: Should this be strdup'd? */
    *out_ast = ast;
    return R_OK;
}
int AstMakeFunction(struct Ast **out_ast, struct Value *function) {
    struct Ast *ast;
    if (!out_ast || !function) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = FunctionNode;;
    ast->Children = NULL;
    ast->NumChildren = 0;
    ast->u.Value = function;
    *out_ast = ast;
    return R_OK;
}

/* Expressions should be a combination of more expressions and terminals. */
int AstMakeBinaryOp(struct Ast **out_ast, struct Ast *lhs, enum AstNodeType op, struct Ast *rhs) {
    struct Ast *ast;
    if (!out_ast || !lhs || !rhs) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = op;
    ast->NumChildren = 2;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = lhs;
    ast->Children[1] = rhs;
    *out_ast = ast;
    return R_OK;
}
int AstMakeUnaryOp(struct Ast **out_ast, enum AstNodeType op, struct Ast *value) {
    struct Ast *ast;
    if (!out_ast || !value) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = op;
    ast->NumChildren = 1;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = value;
    *out_ast = ast;
    return R_OK;
}
int AstMakeAssign(struct Ast **out_ast, struct Ast *lValue, struct Ast *rhs) {
    struct Ast *ast;
    if (!out_ast || !lValue || !rhs) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = AssignExpr;
    ast->NumChildren = 2;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = lValue;
    ast->Children[1] = rhs;
    *out_ast = ast;
    return R_OK;
}
int AstMakeCall(struct Ast **out_ast, char *name, struct Ast *args) {
    struct Ast *ast, *callSymbol;
    int result;
    if (!out_ast || !name) {
        return R_InvalidArgument;
    }
    result = AstMakeSymbol(&callSymbol, name);
    if (R_OK != result) {
        return result;
    }
    ast = malloc(sizeof *ast);
    ast->Type = CallExpr;
    ast->NumChildren = 2;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = callSymbol;
    ast->Children[1] = args;
    *out_ast = ast;
    return R_OK;
}
int AstMakeReturn(struct Ast **out_ast, struct Ast *value) {
    struct Ast *ast;
    if (!out_ast) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = ReturnExpr;
    ast->NumChildren = 1;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = value;
    *out_ast = ast;
    return R_OK;
}
int AstMakeMut(struct Ast **out_ast, struct Ast *names) {
    struct Ast *ast;
    if (!out_ast || !names) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = MutExpr;
    ast->NumChildren = 1;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = names;
    *out_ast = ast;
    return R_OK;
}
int AstMakeConst(struct Ast **out_ast, char *name, struct Ast *value) {
    struct Ast *ast, *symbolName;
    int result;
    if (!out_ast || !name || !value) {
        return R_InvalidArgument;
    }
    result = AstMakeSymbol(&symbolName, name);
    if (R_OK != result) {
        return result;
    }
    ast = malloc(sizeof *ast);
    ast->Type = ConstExpr;
    ast->NumChildren = 2;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = symbolName;
    ast->Children[1] = value;
    *out_ast = ast;
    return R_OK;
}
int AstMakeFor(struct Ast **out_ast, struct Ast *pre, struct Ast *condition, struct Ast *body, struct Ast *post) {
    struct Ast *ast;
    if (!out_ast) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = ForExpr;
    ast->NumChildren = 4;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = pre;
    ast->Children[1] = condition;
    ast->Children[2] = body;
    ast->Children[3] = post;
    *out_ast = ast;
    return R_OK;
}
int AstMakeWhile(struct Ast **out_ast, struct Ast *condition, struct Ast *body) {
    struct Ast *ast;
    if (!out_ast || !condition) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = WhileExpr;
    ast->NumChildren = 2;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = condition;
    ast->Children[1] = body;
    *out_ast = ast;
    return R_OK;
}
int AstMakeIfElse(struct Ast **out_ast, struct Ast *condition, struct Ast *body, struct Ast *elseif) {
    struct Ast *ast;
    if (!out_ast || !condition) {
        return R_InvalidArgument;
    }
    ast = malloc(sizeof *ast);
    ast->Type = IfElseExpr;
    ast->NumChildren = 3;
    ast->Children = malloc(sizeof *(ast->Children) * ast->NumChildren);
    ast->Children[0] = condition;
    ast->Children[1] = body;
    ast->Children[2] = elseif;
    *out_ast = ast;
    return R_OK;
}
