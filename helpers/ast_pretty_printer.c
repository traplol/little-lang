#include "ast_pretty_printer.h"
#include "globals.h"
#include "value.h"

#include "helpers/strings.h"

#include <stdio.h>
#include <stdlib.h>

const char *nodeString[] = {
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
    [UNegExpr] = "u-",
    [ULogicNotExpr] = "u!",
    [AssignExpr] = "assign",
    [ReturnExpr] = "Ret",
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
    return node->u.Value->v.String->CString;
}

const char *fmtFunction(struct Ast *node) {
    return node->u.Value->v.Function->Name;
}

const char *fmtSymbol(struct Ast *node) {
    return node->u.SymbolName;
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
    if (!node) {
        return "<NULL>";
    }
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

int isOperation(struct Ast *node) {
    enum AstNodeType t = node->Type;

    return BooleanNode != t &&
        RealNode != t &&
        IntegerNode != t &&
        StringNode != t &&
        SymbolNode != t &&
        FunctionNode != t;
}

void printNodes(struct Ast *ast);
void printNode(struct Ast *node);

static int tabs = 0;
void printTabs(void) {
    int i;
    for (i = 0; i < tabs; ++i) {
        printf("    ");
    }
}

void printCommaSeparated(struct Ast *node) {
    if (!node) {
        return;
    }
    unsigned int i;
    for (i = 0; i < node->NumChildren; ++i) {
        printNode(node->Children[i]);
        if (i + 1 < node->NumChildren) {
            printf(", ");
        }
    }
}
void printBody(struct Ast *node) {
    printf(" {\n");
    ++tabs;
    printNodes(node);
    --tabs;
    printf("\n");
    printTabs();
    printf("}\n");
}
void printBinaryExpr(struct Ast *node) {
    printf("(%s ", fmtNode(node));
    printNode(node->Children[0]);
    printf(" ");
    printNode(node->Children[1]);
    printf(")");
}
void printUnaryExpr(struct Ast *node) {
    printf("(%s ", fmtNode(node));
    printNode(node->Children[0]);
    printf(")");
}
void printNil(void) {
    printf("nil");
}
void printBoolean(struct Ast *node) {
    printf("%s", fmtNode(node));
}
void printInteger(struct Ast *node) {
    printf("%s", fmtNode(node));
}
void printReal(struct Ast *node) {
    printf("%s", fmtNode(node));
}
void printString(struct Ast *node) {
    printf("\"%s\"", fmtNode(node));
}
void printSymbol(struct Ast *node) {
    printf("%s", fmtNode(node));
}
void printImport(struct Ast *node) {
    struct Ast *import, *as;
    import = node->Children[0];
    as = node->Children[1];
    printf("import ");
    printNode(import);
    if (as) {
        printf(" as ");
        printNode(as);
    }
    printf("\n");
}
void printFunction(struct Ast *node) {
    struct Ast *params, *body;
    printf("def %s", fmtNode(node));
    printf("(");
    params = node->u.Value->v.Function->Params;
    printCommaSeparated(params);
    printf(")");
    body = node->u.Value->v.Function->Body;
    printNode(body);
}
void printCall(struct Ast *node) {
    struct Ast *primary = node->Children[0];
    struct Ast *args = node->Children[1];
    printNode(primary);
    printf("(");
    printCommaSeparated(args);
    printf(")");
}
void printArrayIdx(struct Ast *node) {
    struct Ast *postfix = node->Children[0];
    struct Ast *expr = node->Children[1];
    printNode(postfix);
    printf("[");
    printNode(expr);
    printf("]");
}
void printMemberAccess(struct Ast *node) {
    struct Ast *postfix = node->Children[0];
    struct Ast *identifier = node->Children[1];
    printNode(postfix);
    printf(".");
    printNode(identifier);
}
void printAssign(struct Ast *node) {
    struct Ast *symbol = node->Children[0];
    struct Ast *value = node->Children[1];
    printNode(symbol);
    printf(" = ");
    printNode(value);
}
void printReturn(struct Ast *node) {
    printf("return %s", fmtNode(node));
}
void printMut(struct Ast *node) {
    printf("mut ");
    printCommaSeparated(node->Children[0]);
    printf(" = ");
    printCommaSeparated(node->Children[1]);
}
void printConst(struct Ast *node) {
    printf("const ");
    printNode(node->Children[0]);
    printf(" = ");
    printNode(node->Children[1]);
}
void printFor(struct Ast *node) {
    printf("for ");
    printNode(node->Children[0]); /* pre */
    printf("; ");
    printNode(node->Children[1]); /* cond */
    printf("; ");
    printNode(node->Children[3]); /* post */
    printBody(node->Children[2]); /* body */
}
void printWhile(struct Ast *node) {
    printf("while ");
    printNode(node->Children[0]); /* cond */
    printf(" ");
    printBody(node->Children[1]); /* body */
}
void printIfElse(struct Ast *node) {
    printf("if ");
    printNode(node->Children[0]);
    printBody(node->Children[1]);
    if (node->Children[2]) {
        printTabs();
        printf("else");
        if (IfElseExpr == node->Children[2]->Type) {
            printf(" ");
        }
        printNode(node->Children[2]);
    }
}

int countNodes(struct Ast *ast) {
    int cnt = 0;
    unsigned int i;
    if (!ast || !ast->NumChildren) return 0;
    cnt += ast->NumChildren;
    for (i = 0; i < ast->NumChildren; ++i) {
        cnt += countNodes(ast->Children[i]);
    }
    return cnt;
}

void printNode(struct Ast *node) {
    if (!node) {
        return;
    }
    switch(node->Type) {
        case UNASSIGNED:
            printf("<UNASSIGNED>");
            break;
        case Body:
            printBody(node);
            break;
        case BAddExpr:
        case BSubExpr:
        case BMulExpr:
        case BDivExpr:
        case BModExpr:
        case BPowExpr:
        case BLShift:
        case BRShift:
        case BArithOrExpr:
        case BArithAndExpr:
        case BArithXorExpr:
        case BLogicOrExpr:
        case BLogicAndExpr:
        case BLogicEqExpr:
        case BLogicNotEqExpr:
        case BLogicLtExpr:
        case BLogicLtEqExpr:
        case BLogicGtExpr:
        case BLogicGtEqExpr:
            printBinaryExpr(node);
            break;
        case AssignExpr:
            printAssign(node);
            break;
        case UNegExpr:
        case ULogicNotExpr:
            printUnaryExpr(node);
            break;
        case NilNode:
            printNil();
            break;
        case BooleanNode:
            printBoolean(node);
            break;
        case RealNode:
            printReal(node);
            break;
        case IntegerNode:
            printInteger(node);
            break;
        case StringNode:
            printString(node);
            break;
        case SymbolNode:
            printSymbol(node);
            break;
        case ImportExpr:
            printImport(node);
            break;
        case FunctionNode:
            printFunction(node);
            break;
        case CallExpr:
            printCall(node);
            break;
        case ArrayIdxExpr:
            printArrayIdx(node);
            break;
        case MemberAccessExpr:
            printMemberAccess(node);
            break;
        case ReturnExpr:
            printReturn(node);
            break;
        case MutExpr:
            printMut(node);
            break;
        case ConstExpr:
            printConst(node);
            break;
        case ForExpr:
            printFor(node);
            break;
        case WhileExpr:
            printWhile(node);
            break;
        case IfElseExpr:
            printIfElse(node);
            break;
    }
}

void printNodes(struct Ast *ast) {
    unsigned int i;
    struct Ast *child;
    if (!ast) {
        return;
    }
    for (i = 0; i < ast->NumChildren; ++i) {
        child = ast->Children[i];
        printTabs();
        printNode(child);
        if (i + 1 < ast->NumChildren) {
            printf("\n");
        }
    }
}

void AstPrettyPrint(struct Ast *ast) {
    int nodes = countNodes(ast);
    printf("%d nodes in graph.\n", nodes);
    printNodes(ast);
}
