#include "ast.h"
#include "result.h"

#include <stdlib.h>
#include <stdio.h>

void AstExpandChildren(struct Ast *ast) {
    unsigned int i, capChildren = ast->CapChildren;
    struct Ast **newChildren;

    if (0 == capChildren) {
        capChildren = 2;
    }
    else {
        capChildren *= 2;
    }
    newChildren = calloc(sizeof *newChildren, capChildren);
    for (i = 0; i < ast->CapChildren; ++i) {
        newChildren[i] = ast->Children[i];
    }
    free(ast->Children);
    ast->Children = newChildren;
    ast->CapChildren = capChildren;
}

struct Ast *AstAlloc(unsigned int numChildren) {
    struct Ast *ast = malloc(sizeof *ast);
    ast->CapChildren = numChildren;
    ast->NumChildren = ast->CapChildren;
    if (ast->CapChildren > 0) {
        ast->Children = malloc(sizeof *(ast->Children) * ast->CapChildren);
    }
    else {
        ast->Children = NULL;
    }
    return ast;
}

/************************** Public Functions **************************/

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
    ast = AstAlloc(0);
    ast->Type = BooleanNode;
    ast->u.Value = boolean;
    *out_ast = ast;
    return R_OK;
}
int AstMakeReal(struct Ast **out_ast, struct Value *real) {
    struct Ast *ast;
    if (!out_ast || !real) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(0);
    ast->Type = RealNode;
    ast->u.Value = real;
    *out_ast = ast;
    return R_OK;
}
int AstMakeInteger(struct Ast **out_ast, struct Value *integer) {
    struct Ast *ast;
    if (!out_ast || !integer) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(0);
    ast->Type = IntegerNode;
    ast->u.Value = integer;
    *out_ast = ast;
    return R_OK;
}
int AstMakeString(struct Ast **out_ast, struct Value *string) {
    struct Ast *ast;
    if (!out_ast || !string) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(0);
    ast->Type = StringNode;
    ast->u.Value = string;
    *out_ast = ast;
    return R_OK;
}
int AstMakeSymbol(struct Ast **out_ast, char *name) {
    struct Ast *ast;
    if (!out_ast || !name) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(0);
    ast->Type = SymbolNode;
    ast->u.SymbolName = name; /* FIXME: Should this be strdup'd? */
    *out_ast = ast;
    return R_OK;
}
int AstMakeFunction(struct Ast **out_ast, struct Value *function) {
    struct Ast *ast;
    if (!out_ast || !function) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(0);
    ast->Type = FunctionNode;;
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
    ast = AstAlloc(2);
    ast->Type = op;
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
    ast = AstAlloc(1);
    ast->Type = op;
    ast->Children[0] = value;
    *out_ast = ast;
    return R_OK;
}
int AstMakeAssign(struct Ast **out_ast, struct Ast *lValue, struct Ast *rhs) {
    struct Ast *ast;
    if (!out_ast || !lValue || !rhs) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(2);
    ast->Type = AssignExpr;
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
    ast = AstAlloc(2);
    ast->Type = CallExpr;
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
    ast = AstAlloc(1);
    ast->Type = ReturnExpr;
    ast->Children[0] = value;
    *out_ast = ast;
    return R_OK;
}
int AstMakeMut(struct Ast **out_ast, struct Ast *names, struct Ast *values) {
    struct Ast *ast;
    if (!out_ast || !names || !values) {
        return R_InvalidArgument;
    }
    if (0 == names->NumChildren || 0 == values->NumChildren) {
        return R_InvalidArgument;
    }
    if (names->NumChildren != values->NumChildren) {
        return R_InvalidArgument;
    }
    ast = AstAlloc(2);
    ast->Type = MutExpr;
    ast->Children[0] = names;
    ast->Children[1] = values;
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
    ast = AstAlloc(2);
    ast->Type = ConstExpr;
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
    ast = AstAlloc(4);
    ast->Type = ForExpr;
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
    ast = AstAlloc(2);
    ast->Type = WhileExpr;
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
    ast = AstAlloc(3);
    ast->Type = IfElseExpr;
    ast->Children[0] = condition;
    ast->Children[1] = body;
    ast->Children[2] = elseif;
    *out_ast = ast;
    return R_OK;
}

int AstMakeBlank(struct Ast **out_ast) {
    struct Ast *ast;
    if (!out_ast) {
        return R_InvalidArgument;
    }
    ast = calloc(sizeof *ast, 1);
    ast->Type = UNASSIGNED;
    *out_ast = ast;
    return R_OK;
}

int AstAppendChild(struct Ast *ast, struct Ast *child) {
    if (!ast) {
        return R_InvalidArgument;
    }
    if (ast->NumChildren == ast->CapChildren) {
        AstExpandChildren(ast);
    }
    ast->Children[ast->NumChildren++] = child;
    return R_OK;
}

