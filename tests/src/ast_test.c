#include "../src/type_info.c"
#include "../src/value.c"
#include "../src/ast.c"
#include "../helpers/strings.c"

#include "c_test.h"

struct Value *value;
struct Ast *ast1, *ast2, *ast3, *ast4, *ast5;
char *name;
void setup(void) {
    value = malloc(sizeof *value);
    ast1 = malloc(sizeof *ast1);
    ast2 = malloc(sizeof *ast2);
    ast3 = malloc(sizeof *ast3);
    ast4 = malloc(sizeof *ast4);
    ast5 = malloc(sizeof *ast5);
    name = strdup("the_name");
}

void done(void) {
    free(value);
    free(ast1);
    free(ast2);
    free(ast3);
    free(ast4);
    free(ast5);
    free(name);
}

TEST(AstMakeBoolean) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeBoolean(&ast, value), "AstMakeBoolean failed.");
    assert_eq(R_InvalidArgument, AstMakeBoolean(&ast, NULL), "AstMakeBoolean did not fail.");
    assert_eq(R_InvalidArgument, AstMakeBoolean(NULL, value), "AstMakeBoolean did not fail.");
    //AstFree(ast);
    free(ast);
}

TEST(AstMakeReal) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeReal(&ast, value), "AstMakeReal failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeReal(&ast, NULL), "AstMakeReal did not fail.");
    assert_eq(R_InvalidArgument, AstMakeReal(NULL, value), "AstMakeReal did not fail.");
}

TEST(AstMakeInteger) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeInteger(&ast, value), "AstMakeInteger failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeInteger(&ast, NULL), "AstMakeInteger did not fail.");
    assert_eq(R_InvalidArgument, AstMakeInteger(NULL, value), "AstMakeInteger did not fail.");
}

TEST(AstMakeString) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeString(&ast, value), "AstMakeString failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeString(&ast, NULL), "AstMakeString did not fail.");
    assert_eq(R_InvalidArgument, AstMakeString(NULL, value), "AstMakeString did not fail.");
}

TEST(AstMakeSymbol) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeSymbol(&ast, name), "AstMakeSymbol failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeSymbol(&ast, NULL), "AstMakeSymbol did not fail.");
    assert_eq(R_InvalidArgument, AstMakeSymbol(NULL, name), "AstMakeSymbol did not fail.");
}

TEST(AstMakeFunction) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeFunction(&ast, value), "AstMakeFunction failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeFunction(&ast, NULL), "AstMakeFunction did not fail.");
    assert_eq(R_InvalidArgument, AstMakeFunction(NULL, value), "AstMakeFunction did not fail.");
}

TEST(AstMakeBinaryOp) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeBinaryOp(&ast, ast1, BAddExpr, ast2), "AstMakeBinaryOp failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeBinaryOp(NULL, ast1, BAddExpr, ast2), "AstMakeBinaryOp did not fail.");
    assert_eq(R_InvalidArgument, AstMakeBinaryOp(&ast, NULL, BAddExpr, ast2), "AstMakeBinaryOp did not fail.");
    assert_eq(R_InvalidArgument, AstMakeBinaryOp(&ast, ast1, BAddExpr, NULL), "AstMakeBinaryOp did not fail.");
}

TEST(AstMakeUnaryOp) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeUnaryOp(&ast, UNegExpr, ast1), "AstMakeUnaryOp failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeUnaryOp(NULL, UNegExpr, ast1), "AstMakeUnaryOp did not fail.");
    assert_eq(R_InvalidArgument, AstMakeUnaryOp(&ast, UNegExpr, NULL), "AstMakeUnaryOp did not fail.");
}

TEST(AstMakeAssign) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeAssign(&ast, ast1, ast2), "AstMakeAssign failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeAssign(NULL, ast1, ast2), "AstMakeAssign failed.");
    assert_eq(R_InvalidArgument, AstMakeAssign(&ast, NULL, ast2), "AstMakeAssign did not fail.");
    assert_eq(R_InvalidArgument, AstMakeAssign(&ast, ast1, NULL), "AstMakeAssign did not fail.");
}

TEST(AstMakeCall) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeCall(&ast, name, ast1), "AstMakeCall failed.");
    free(ast->Children[0]); free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeCall(&ast, name, NULL), "AstMakeCall with no args failed.");
    free(ast->Children[0]); free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeCall(NULL, name, ast1), "AstMakeCall did not fail.");
    assert_eq(R_InvalidArgument, AstMakeCall(&ast, NULL, ast1), "AstMakeCall did not fail.");
}

TEST(AstMakeReturn) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeReturn(&ast, ast1), "AstMakeReturn failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeReturn(&ast, NULL), "AstMakeReturn with no value failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeReturn(NULL, ast1), "AstMakeReturn did not fail.");
}

TEST(AstMakeMut) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeMut(&ast, ast1), "AstMakeMut failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeMut(NULL, ast1), "AstMakeMut did not fail.");
    assert_eq(R_InvalidArgument, AstMakeMut(&ast, NULL), "AstMakeMut did not fail with no names.");
}

TEST(AstMakeConst) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeConst(&ast, name, ast1), "AstMakeConst failed.");
    free(ast->Children[0]); free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeConst(NULL, name, ast1), "AstMakeConst did not fail.");
    assert_eq(R_InvalidArgument, AstMakeConst(&ast, NULL, ast1), "AstMakeConst did not fail.");
    assert_eq(R_InvalidArgument, AstMakeConst(&ast, name, NULL), "AstMakeConst did not fail.");
}

TEST(AstMakeFor) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeFor(&ast, ast1, ast2, ast3, ast4), "AstMakeFor failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, NULL, ast2, ast3, ast4), "AstMakeFor failed with no pre.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, ast1, NULL, ast3, ast4), "AstMakeFor failed with no condition.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, ast1, ast2, NULL, ast4), "AstMakeFor failed with no body.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, ast1, ast2, ast3, NULL), "AstMakeFor failed with no post.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, NULL, NULL, NULL, NULL), "AstMakeFor failed with nothing.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeFor(NULL, ast1, ast2, ast3, ast4), "AstMakeFor did not fail.");
}

TEST(AstMakeWhile) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeWhile(&ast, ast1, ast2), "AstMakeWhile failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeWhile(&ast, ast1, NULL), "AstMakeWhile failed with no body");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeWhile(NULL, ast1, ast2), "AstMakeWhile did not fail.");
    assert_eq(R_InvalidArgument, AstMakeWhile(&ast, NULL, ast2), "AstMakeWhile did not fail with no condition.");
}

TEST(AstMakeIfElse) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, ast2, ast3), "AstMakeIfElse failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, NULL, ast3), "AstMakeIfElse failed with no body.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, ast2, NULL), "AstMakeIfElse failed with no elseif.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, NULL, NULL), "AstMakeIfElse failed with no body or elseif.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeIfElse(NULL, ast1, ast2, ast3), "AstMakeIfElse did not fail.");
    assert_eq(R_InvalidArgument, AstMakeIfElse(NULL, NULL, ast2, ast3), "AstMakeIfElse did not fail with no condition.");
}

int main() {
    setup();
    TEST_RUN(AstMakeBoolean);
    TEST_RUN(AstMakeReal);
    TEST_RUN(AstMakeInteger);
    TEST_RUN(AstMakeString);
    TEST_RUN(AstMakeSymbol);
    TEST_RUN(AstMakeFunction);
    TEST_RUN(AstMakeBinaryOp);
    TEST_RUN(AstMakeUnaryOp);
    TEST_RUN(AstMakeAssign);
    TEST_RUN(AstMakeCall);
    TEST_RUN(AstMakeReturn);
    TEST_RUN(AstMakeMut);
    TEST_RUN(AstMakeConst);
    TEST_RUN(AstMakeFor);
    TEST_RUN(AstMakeWhile);
    TEST_RUN(AstMakeIfElse);
    done();
}
