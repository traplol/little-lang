#include "../src/globals.c"
#include "../src/type_info.c"
#include "../src/value.c"
#include "../src/ast.c"
#include "../helpers/strings.c"

#include "c_test.h"

struct Value *value;
struct Ast *ast1, *ast2, *ast3, *ast4, *ast5;
char *name;
struct SrcLoc srcLoc = {"test.ll", -1, -1};
void setup(void) {
    GlobalsInit();
    value = malloc(sizeof *value);
    name = strdup("the_name");

    AstMakeBlank(&ast1);
    AstMakeBlank(&ast2);
    AstMakeBlank(&ast3);
    AstMakeBlank(&ast4);
    AstMakeBlank(&ast5);
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
    assert_eq(R_OK, AstMakeBoolean(&ast, value, srcLoc), "AstMakeBoolean failed.");
    assert_eq(R_InvalidArgument, AstMakeBoolean(&ast, NULL, srcLoc), "AstMakeBoolean did not fail.");
    assert_eq(R_InvalidArgument, AstMakeBoolean(NULL, value, srcLoc), "AstMakeBoolean did not fail.");
    //AstFree(ast);
    free(ast);
}

TEST(AstMakeReal) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeReal(&ast, value, srcLoc), "AstMakeReal failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeReal(&ast, NULL, srcLoc), "AstMakeReal did not fail.");
    assert_eq(R_InvalidArgument, AstMakeReal(NULL, value, srcLoc), "AstMakeReal did not fail.");
}

TEST(AstMakeInteger) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeInteger(&ast, value, srcLoc), "AstMakeInteger failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeInteger(&ast, NULL, srcLoc), "AstMakeInteger did not fail.");
    assert_eq(R_InvalidArgument, AstMakeInteger(NULL, value, srcLoc), "AstMakeInteger did not fail.");
}

TEST(AstMakeString) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeString(&ast, value, srcLoc), "AstMakeString failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeString(&ast, NULL, srcLoc), "AstMakeString did not fail.");
    assert_eq(R_InvalidArgument, AstMakeString(NULL, value, srcLoc), "AstMakeString did not fail.");
}

TEST(AstMakeSymbol) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeSymbol(&ast, name, srcLoc), "AstMakeSymbol failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeSymbol(&ast, NULL, srcLoc), "AstMakeSymbol did not fail.");
    assert_eq(R_InvalidArgument, AstMakeSymbol(NULL, name, srcLoc), "AstMakeSymbol did not fail.");
}

TEST(AstMakeFunction) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeFunction(&ast, value, srcLoc), "AstMakeFunction failed.");
    free(ast);
    assert_eq(R_InvalidArgument, AstMakeFunction(&ast, NULL, srcLoc), "AstMakeFunction did not fail.");
    assert_eq(R_InvalidArgument, AstMakeFunction(NULL, value, srcLoc), "AstMakeFunction did not fail.");
}

TEST(AstMakeBinaryOp) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeBinaryOp(&ast, ast1, BAddExpr, ast2, srcLoc), "AstMakeBinaryOp failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeBinaryOp(NULL, ast1, BAddExpr, ast2, srcLoc), "AstMakeBinaryOp did not fail.");
    assert_eq(R_InvalidArgument, AstMakeBinaryOp(&ast, NULL, BAddExpr, ast2, srcLoc), "AstMakeBinaryOp did not fail.");
    assert_eq(R_InvalidArgument, AstMakeBinaryOp(&ast, ast1, BAddExpr, NULL, srcLoc), "AstMakeBinaryOp did not fail.");
}

TEST(AstMakeUnaryOp) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeUnaryOp(&ast, UNegExpr, ast1, srcLoc), "AstMakeUnaryOp failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeUnaryOp(NULL, UNegExpr, ast1, srcLoc), "AstMakeUnaryOp did not fail.");
    assert_eq(R_InvalidArgument, AstMakeUnaryOp(&ast, UNegExpr, NULL, srcLoc), "AstMakeUnaryOp did not fail.");
}

TEST(AstMakeAssign) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeAssign(&ast, ast1, ast2, srcLoc), "AstMakeAssign failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeAssign(NULL, ast1, ast2, srcLoc), "AstMakeAssign failed.");
    assert_eq(R_InvalidArgument, AstMakeAssign(&ast, NULL, ast2, srcLoc), "AstMakeAssign did not fail.");
    assert_eq(R_InvalidArgument, AstMakeAssign(&ast, ast1, NULL, srcLoc), "AstMakeAssign did not fail.");
}

TEST(AstMakeCall) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeCall(&ast, ast1, ast2, srcLoc), "AstMakeCall failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeCall(&ast, ast1, NULL, srcLoc), "AstMakeCall with no args failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeCall(NULL, ast1, ast2, srcLoc), "AstMakeCall did not fail.");
    assert_eq(R_InvalidArgument, AstMakeCall(&ast, NULL, ast2, srcLoc), "AstMakeCall did not fail.");
}

TEST(AstMakeReturn) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeReturn(&ast, ast1, srcLoc), "AstMakeReturn failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeReturn(&ast, NULL, srcLoc), "AstMakeReturn with no value failed.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeReturn(NULL, ast1, srcLoc), "AstMakeReturn did not fail.");
}

TEST(AstMakeMut) {
    struct Ast *ast;
    struct Ast *tmp1, *tmp2;
    AstMakeBlank(&tmp1);
    AstAppendChild(tmp1, ast1);
    AstMakeBlank(&tmp2);
    AstAppendChild(tmp2, ast2);
    assert_eq(R_OK, AstMakeMut(&ast, tmp1, tmp2, srcLoc), "AstMakeMut failed.");
    free(ast->Children); free(ast);

    assert_eq(R_InvalidArgument, AstMakeMut(&ast, tmp1, ast2, srcLoc), "AstMakeMut should fail when either arg has no children");
    assert_eq(R_InvalidArgument, AstMakeMut(&ast, ast1, tmp2, srcLoc), "AstMakeMut should fail when either arg has no children");

    AstAppendChild(tmp2, ast2);
    assert_eq(R_InvalidArgument, AstMakeMut(&ast, ast1, tmp2, srcLoc), "AstMakeMut should fail when args don't have same number of children.");

    assert_eq(R_InvalidArgument, AstMakeMut(NULL, tmp1, tmp2, srcLoc), "AstMakeMut did not fail.");
    assert_eq(R_InvalidArgument, AstMakeMut(&ast, NULL, tmp2, srcLoc), "AstMakeMut did not fail with no names.");
    assert_eq(R_InvalidArgument, AstMakeMut(&ast, tmp1, NULL, srcLoc), "AstMakeMut did not fail with no names.");

    free(tmp1->Children);
    free(tmp1);
    free(tmp2->Children);
    free(tmp2);
}

TEST(AstMakeConst) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeConst(&ast, name, ast1, srcLoc), "AstMakeConst failed.");
    free(ast->Children[0]); free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeConst(NULL, name, ast1, srcLoc), "AstMakeConst did not fail.");
    assert_eq(R_InvalidArgument, AstMakeConst(&ast, NULL, ast1, srcLoc), "AstMakeConst did not fail.");
    assert_eq(R_InvalidArgument, AstMakeConst(&ast, name, NULL, srcLoc), "AstMakeConst did not fail.");
}

TEST(AstMakeFor) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeFor(&ast, ast1, ast2, ast3, ast4, srcLoc), "AstMakeFor failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, NULL, ast2, ast3, ast4, srcLoc), "AstMakeFor failed with no pre.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, ast1, NULL, ast3, ast4, srcLoc), "AstMakeFor failed with no condition.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, ast1, ast2, NULL, ast4, srcLoc), "AstMakeFor failed with no body.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, ast1, ast2, ast3, NULL, srcLoc), "AstMakeFor failed with no post.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeFor(&ast, NULL, NULL, NULL, NULL, srcLoc), "AstMakeFor failed with nothing.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeFor(NULL, ast1, ast2, ast3, ast4, srcLoc), "AstMakeFor did not fail.");
}

TEST(AstMakeWhile) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeWhile(&ast, ast1, ast2, srcLoc), "AstMakeWhile failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeWhile(&ast, ast1, NULL, srcLoc), "AstMakeWhile failed with no body");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeWhile(NULL, ast1, ast2, srcLoc), "AstMakeWhile did not fail.");
    assert_eq(R_InvalidArgument, AstMakeWhile(&ast, NULL, ast2, srcLoc), "AstMakeWhile did not fail with no condition.");
}

TEST(AstMakeIfElse) {
    struct Ast *ast;
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, ast2, ast3, srcLoc), "AstMakeIfElse failed.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, NULL, ast3, srcLoc), "AstMakeIfElse failed with no body.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, ast2, NULL, srcLoc), "AstMakeIfElse failed with no elseif.");
    free(ast->Children); free(ast);
    assert_eq(R_OK, AstMakeIfElse(&ast, ast1, NULL, NULL, srcLoc), "AstMakeIfElse failed with no body or elseif.");
    free(ast->Children); free(ast);
    assert_eq(R_InvalidArgument, AstMakeIfElse(NULL, ast1, ast2, ast3, srcLoc), "AstMakeIfElse did not fail.");
    assert_eq(R_InvalidArgument, AstMakeIfElse(NULL, NULL, ast2, ast3, srcLoc), "AstMakeIfElse did not fail with no condition.");
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
