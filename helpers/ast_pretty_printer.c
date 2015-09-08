#include "ast_pretty_printer.h"
#include "globals.h"
#include "value.h"

#include "helpers/strings.h"

#include <stdio.h>
#include <stdlib.h>

const char *nodeString[] = {
    [UNASSIGNED] = "UNASSIGNED",
    [BAddExpr] = "+",
    [BSubExpr] = "-",
    [BMulExpr] = "*",
    [BDivExpr] = "/",
    [BModExpr] = "%",
    [BPowExpr] = "**",
    [BLShift] = "<<",
    [BRShift] = ">>",
    [BArithOrExpr] = "|",
    [BArithAndExpr] = "&",
    [BArithXorExpr] = "^",
    [BLogicOrExpr] = "||",
    [BLogicAndExpr] = "&&",
    [BLogicEqExpr] = "==",
    [BLogicNotEqExpr] = "!=",
    [BLogicLtExpr] = "<",
    [BLogicLtEqExpr] = "<=",
    [BLogicGtExpr] = ">",
    [BLogicGtEqExpr] = ">=",
    [UNegExpr] = "-",
    [ULogicNotExpr] = "!",
    [AssignExpr] = "=",
    [BooleanNode] = "THIS SHOULDNT BE SEEN",
    [RealNode] = "THIS SHOULDNT BE SEEN",
    [IntegerNode] = "THIS SHOULDNT BE SEEN",
    [StringNode] = "THIS SHOULDNT BE SEEN",
    [SymbolNode] = "Sym",
    [FunctionNode] = "THIS SHOULDNT BE SEEN",
    [CallExpr] = "Call",
    [ReturnExpr] = "Ret",
    [MutExpr] = "Mut",
    [ConstExpr] = "Const",
    [ForExpr] = "For",
    [WhileExpr] = "While",
    [IfElseExpr] = "IfElse",
};

const char *fmtBooleanLiteral(struct Ast *node) {
    if (&g_TheTrueValue == node->u.Value) {
        return "true";
    }
    if (&g_TheFalseValue == node->u.Value) {
        return "false";
    }
    return "<BAD BOOL>";
}
const char *fmtRealLiteral(struct Ast *node) {
    char buf[80];
    double val = node->u.Value->v.Real;
    snprintf(buf, 80, "%f", val);
    return strdup(buf);
}
const char *fmtIntegerLiteral(struct Ast *node) {
    char buf[80];
    int val = node->u.Value->v.Integer;
    snprintf(buf, 80, "%d", val);
    return strdup(buf);
}
const char *fmtStringLiteral(struct Ast *node) {
    char buf[10];
    char *str = strndup(node->u.Value->v.UserObject, 5);
    snprintf(buf, 10, "S\"%s\"", str);
    free(str);
    return strdup(buf);
}

const char *fmtFunction(struct Ast *node) {
    char buf[10];
    char *str = strndup(node->u.Value->v.Function->Name, 5);
    snprintf(buf, 10, "F(%s)", str);
    free(str);
    return strdup(buf);
}

const char *fmtSymbol(struct Ast *node) {
    char buf[10];
    char *str = strndup(node->u.SymbolName, 5);
    snprintf(buf, 10, "S(%s)", str);
    free(str);
    return strdup(buf);
}

const char *fmtNodeLiteral(struct Ast *node) {
    switch (node->Type) {
        default: return "<BAD>";
        case BooleanNode: return fmtBooleanLiteral(node);
        case RealNode: return fmtRealLiteral(node);
        case IntegerNode: return fmtIntegerLiteral(node);
        case StringNode: return fmtStringLiteral(node);
    }
}

const char *fmtNode(struct Ast *node) {
    switch (node->Type) {
        default: return nodeString[node->Type];
        case FunctionNode: return fmtFunction(node);
        case SymbolNode: return fmtSymbol(node);
        case BooleanNode:
        case RealNode:
        case IntegerNode:
        case StringNode:
            return fmtNodeLiteral(node);
    }
}


int countNodes(struct Ast *ast) {
    int cnt = 0;
    unsigned int i;
    if (!ast) return 0;
    cnt += ast->NumChildren;
    for (i = 0; i < ast->NumChildren; ++i) {
        cnt += countNodes(ast->Children[i]);
    }
    return cnt;
}

void printNodes(struct Ast *ast) {
    unsigned int i;
    struct Ast *child;
    if (!ast) {
        return;
    }
    if (UNASSIGNED == ast->Type) {
        printf("\n");
    }
    printf("%s ", fmtNode(ast));
    for (i = 0; i < ast->NumChildren; ++i) {
        child = ast->Children[i];
        printNodes(child);
    }
}

void AstPrettyPrint(struct Ast *ast) {
    int nodes = countNodes(ast);
    printf("%d nodes in graph.\n", nodes);
    printNodes(ast);
}
