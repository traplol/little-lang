#include "interpreter.h"
#include "symbol_table.h"
#include "globals.h"
#include "runtime/registrar.h"
#include "value.h"
#include "result.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define TO_BOOLEAN(b) (b) ? &g_TheTrueValue : &g_TheFalseValue

/* Forward declarations. */
struct Value *InterpreterRunAst(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoBody(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoAdd(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoSub(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoMul(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoDiv(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoMod(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoPow(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLShift(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoRShift(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoArithOr(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoArithAnd(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoXorExpr(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicOr(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicAnd(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicEq(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicNotEq(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicLt(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicLtEq(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicGt(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicGtEq(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoNegate(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoLogicNot(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoAssign(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoSymbol(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoDefFunction(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoCall(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoArrayIdx(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoMemberAccess(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoReturn(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoMut(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoConst(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoFor(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoWhile(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoIfElse(struct LittleLangMachine *llm, struct Ast *ast);
struct Value *InterpreterDoBoolean(struct Ast *ast);
struct Value *InterpreterDoReal(struct Ast *ast);
struct Value *InterpreterDoInteger(struct Ast *ast);
struct Value *InterpreterDoString(struct Ast *ast);

/* Function definitions */
struct Value *InterpreterDoBody(struct LittleLangMachine *llm, struct Ast *ast) {
    unsigned int i;
    struct Value *value = &g_TheNilValue;
    for (i = 0; i < ast->NumChildren; ++i) {
        value = InterpreterRunAst(llm, ast->Children[i]);
    }
    return value;
}
struct Value *InterpreterDoAdd(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    struct Value *value = ValueAlloc();
    int isReal;
    union {
        int Integer;
        double Real;
    } result;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        isReal = 1;
        result.Real = lhs->v.Real + rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        isReal = 0;
        result.Integer = lhs->v.Integer + rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        isReal = 1;
        result.Real = lhs->v.Integer + rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        isReal = 1;
        result.Real = lhs->v.Real + rhs->v.Integer;
    }

    if (isReal) {
        ValueMakeReal(value, result.Real);
    }
    else {
        ValueMakeInteger(value, result.Integer);
    }
    return value;

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoSub(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    struct Value *value = ValueAlloc();
    int isReal;
    union {
        int Integer;
        double Real;
    } result;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        isReal = 1;
        result.Real = lhs->v.Real - rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        isReal = 0;
        result.Integer = lhs->v.Integer - rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        isReal = 1;
        result.Real = lhs->v.Integer - rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        isReal = 1;
        result.Real = lhs->v.Real - rhs->v.Integer;
    }

    if (isReal) {
        ValueMakeReal(value, result.Real);
    }
    else {
        ValueMakeInteger(value, result.Integer);
    }
    return value;

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoMul(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    struct Value *value = ValueAlloc();
    int isReal;
    union {
        int Integer;
        double Real;
    } result;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        isReal = 1;
        result.Real = lhs->v.Real * rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        isReal = 0;
        result.Integer = lhs->v.Integer * rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        isReal = 1;
        result.Real = lhs->v.Integer * rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        isReal = 1;
        result.Real = lhs->v.Real * rhs->v.Integer;
    }

    if (isReal) {
        ValueMakeReal(value, result.Real);
    }
    else {
        ValueMakeInteger(value, result.Integer);
    }
    return value;

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoDiv(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    struct Value *value = ValueAlloc();
    int isReal;
    union {
        int Integer;
        double Real;
    } result;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        isReal = 1;
        result.Real = lhs->v.Real / rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        isReal = 0;
        result.Integer = lhs->v.Integer / rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        isReal = 1;
        result.Real = lhs->v.Integer / rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        isReal = 1;
        result.Real = lhs->v.Real / rhs->v.Integer;
    }

    if (isReal) {
        ValueMakeReal(value, result.Real);
    }
    else {
        ValueMakeInteger(value, result.Integer);
    }
    return value;

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoMod(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    struct Value *value = ValueAlloc();
    int isReal;
    union {
        int Integer;
        double Real;
    } result;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        isReal = 1;
        result.Real = fmod(lhs->v.Real, rhs->v.Real);
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        isReal = 0;
        result.Integer = lhs->v.Integer % rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        isReal = 1;
        result.Real = fmod(lhs->v.Integer, rhs->v.Real);
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        isReal = 1;
        result.Real = fmod(lhs->v.Real, rhs->v.Integer);
    }

    if (isReal) {
        ValueMakeReal(value, result.Real);
    }
    else {
        ValueMakeInteger(value, result.Integer);
    }
    return value;

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoPow(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoLShift(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoRShift(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoArithOr(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoArithAnd(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoXorExpr(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicOr(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs, *rhs;
    lhs = InterpreterRunAst(llm, ast->Children[0]);
    if (&g_TheTrueValue == lhs) {
        return &g_TheTrueValue;
    }
    rhs = InterpreterRunAst(llm, ast->Children[1]);
    if (&g_TheTrueValue == rhs) {
        return &g_TheTrueValue;
    }
    if (&g_TheFalseValue == rhs) {
        return &g_TheFalseValue;
    }
    /* TODO: Runtime error */
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicAnd(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *lhs, *rhs;
    lhs = InterpreterRunAst(llm, ast->Children[0]);
    if (&g_TheFalseValue == lhs) {
        return &g_TheFalseValue;
    }
    rhs = InterpreterRunAst(llm, ast->Children[1]);
    if (&g_TheFalseValue == rhs) {
        return &g_TheFalseValue;
    }
    if (&g_TheTrueValue == lhs && lhs == rhs) {
        return &g_TheTrueValue;
    }
    /* TODO: Runtime error */
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicEq(struct LittleLangMachine *llm, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    double epsilon = 1.11e-16; /* TODO: get rid of this magic number. */
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        result = fabs(lhs->v.Real - rhs->v.Real) < epsilon;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        result = lhs->v.Integer == rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        result = fabs(lhs->v.Integer - rhs->v.Real) < epsilon;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        result = fabs(lhs->v.Real - rhs->v.Integer) < epsilon;
    }
    return TO_BOOLEAN(result);

non_numeric_types:
    if (lhs == rhs) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
struct Value *InterpreterDoLogicNotEq(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *value = InterpreterDoLogicEq(llm, ast);
    if (&g_TheTrueValue == value) {
        return &g_TheFalseValue;
    }
    else if (&g_TheFalseValue == value) {
        return &g_TheTrueValue;
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicLt(struct LittleLangMachine *llm, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        result = lhs->v.Real < rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        result = lhs->v.Integer < rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        result = lhs->v.Integer < rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        result = lhs->v.Real < rhs->v.Integer;
    }
    return TO_BOOLEAN(result);

non_numeric_types:
    if (lhs != rhs) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
struct Value *InterpreterDoLogicLtEq(struct LittleLangMachine *llm, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        result = lhs->v.Real <= rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        result = lhs->v.Integer <= rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        result = lhs->v.Integer <= rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        result = lhs->v.Real <= rhs->v.Integer;
    }
    return TO_BOOLEAN(result);

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicGt(struct LittleLangMachine *llm, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        result = lhs->v.Real > rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        result = lhs->v.Integer > rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        result = lhs->v.Integer > rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        result = lhs->v.Real > rhs->v.Integer;
    }
    return TO_BOOLEAN(result);

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicGtEq(struct LittleLangMachine *llm, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[1]);
    enum TypeInfoType lhsType = lhs->TypeInfo->Type;
    enum TypeInfoType rhsType = rhs->TypeInfo->Type;
    if ((TypeReal == lhsType || TypeInteger == lhsType) &&
        (TypeReal == rhsType || TypeInteger == rhsType)) {
        goto numeric_types;
    }
    else {
        goto non_numeric_types;
    }

numeric_types:
    if (TypeReal == lhsType && lhsType == rhsType) {
        result = lhs->v.Real >= rhs->v.Real;
    }
    else if (TypeInteger == lhsType && lhsType == rhsType) {
        result = lhs->v.Integer >= rhs->v.Integer;
    }
    else if (TypeInteger == lhsType && TypeReal == rhsType){
        result = lhs->v.Integer >= rhs->v.Real;
    }
    else { /* TypeReal == lhsType && TypeInteger == rhsType */
        result = lhs->v.Real >= rhs->v.Integer;
    }
    return TO_BOOLEAN(result);

non_numeric_types:
    return &g_TheNilValue;
}
struct Value *InterpreterDoNegate(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[0]);
    struct Value *value;
    if (TypeReal == rhs->TypeInfo->Type) {
        value = ValueAlloc();
        ValueMakeReal(value, rhs->v.Real * -1);
        return value;
    }
    else if (TypeInteger == rhs->TypeInfo->Type){
        value = ValueAlloc();
        ValueMakeInteger(value, rhs->v.Integer * -1);
        return value;
    }
    /* TODO: Runtime Error */
    return &g_TheNilValue;

}
struct Value *InterpreterDoLogicNot(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *rhs = InterpreterRunAst(llm, ast->Children[0]);
    if (&g_TheTrueValue == rhs) {
        return &g_TheFalseValue;
    }
    if (&g_TheFalseValue == rhs) {
        return &g_TheTrueValue;
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoAssign(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Symbol *symbol;
    struct Value *value;
    char *symName = ast->Children[0]->u.SymbolName;
    if (!SymbolTableFindNearest(llm->CurrentScope, symName, &symbol)) {
        printf("Trying to assign to undefined symbol: '%s' at %s:%d:%d\n",
               symbol->Key,
               ast->SrcLoc.Filename,
               ast->SrcLoc.LineNumber,
               ast->SrcLoc.ColumnNumber);
        return &g_TheNilValue;
    }
    if (!symbol->IsMutable) {
        printf("Trying to assign to const symbol: '%s' at %s:%d:%d\n",
               symbol->Key,
               ast->SrcLoc.Filename,
               ast->SrcLoc.LineNumber,
               ast->SrcLoc.ColumnNumber);
        return &g_TheNilValue;
    }
    value = InterpreterRunAst(llm, ast->Children[1]);
    symbol->Value = value;
    return value;
}
struct Value *InterpreterDoBoolean(struct Ast *ast){
    return ast->u.Value;
}
struct Value *InterpreterDoReal(struct Ast *ast){
    return ast->u.Value;
}
struct Value *InterpreterDoInteger(struct Ast *ast){
    return ast->u.Value;
}
struct Value *InterpreterDoString(struct Ast *ast){
    return ast->u.Value;
}
struct Value *InterpreterDoSymbol(struct LittleLangMachine *llm, struct Ast *ast){
    struct Symbol *sym;
    if (!SymbolTableFindNearest(llm->CurrentScope, ast->u.SymbolName, &sym)) {
        printf("Undefined symbol '%s'.\n", ast->u.SymbolName);
        return &g_TheNilValue;
    }
    return sym->Value;
}
struct Value *InterpreterDoDefFunction(struct LittleLangMachine *llm, struct Ast *ast){
    return &g_TheNilValue;
}
struct Value *InterpreterDoCallBuiltinFn(struct LittleLangMachine *llm, struct Value *function, struct Ast *args) {
    unsigned int argc, i;
    struct Value **argv;
    if (args && args->NumChildren > 0) {
        argc = args->NumChildren;
        argv = malloc(sizeof(*argv) * argc);
        for (i = 0; i < argc; ++i) {
            argv[i] = InterpreterRunAst(llm, args->Children[i]);
        }
    }
    else {
        argc = 0;
        argv = NULL;
    }
    return function->v.BuiltinFn->Fn(argc, argv);
}
struct Value *InterpreterDoCallFunction(struct LittleLangMachine *llm, struct Value *function, struct Ast *args, struct SrcLoc srcLoc) {
    unsigned int i;
    struct Value *returnValue, *arg;
    struct Ast *params, *body, *param;
    struct Function *fn = function->v.Function;
    params = fn->Params;
    body = fn->Body;
    if (args->NumChildren < params->NumChildren) {
        /* TODO: Throw proper error. */
        printf("Wrong number of args for call: '%s' at %s:%d:%d\n",
               fn->Name,
               srcLoc.Filename,
               srcLoc.LineNumber,
               srcLoc.ColumnNumber);
        return &g_TheNilValue;
    }
    else if (args->NumChildren > params->NumChildren && !fn->IsVarArgs) {
        /* TODO: Throw proper error. */
        printf("Wrong number of args for call: '%s' at %s:%d:%d\n",
               fn->Name,
               srcLoc.Filename,
               srcLoc.LineNumber,
               srcLoc.ColumnNumber);
        return &g_TheNilValue;
    }
    SymbolTablePushScope(&(llm->CurrentScope));
    /* Setup params */
    /* TODO: Handle varargs */
    if (params) {
        for (i = 0; i < params->NumChildren; ++i) {
            arg = InterpreterRunAst(llm, args->Children[i]);
            param = params->Children[i];
            SymbolTableInsert(llm->CurrentScope, arg, param->u.SymbolName, 1, param->SrcLoc);
        }
    }
    /* Execute body. */
    for (i = 0; i < body->NumChildren; ++i) {
        returnValue = InterpreterRunAst(llm, body->Children[i]);
    }
    SymbolTablePopScope(&(llm->CurrentScope));
    return returnValue;
}
struct Value *InterpreterDoCall(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Value *func = InterpreterRunAst(llm, ast->Children[0]);
    if (&g_TheNilValue == func) {
        return &g_TheNilValue;
    }
    if (func->IsBuiltInFn) {
        return InterpreterDoCallBuiltinFn(llm, func, ast->Children[1]);
    }
    return InterpreterDoCallFunction(llm, func, ast->Children[1], ast->SrcLoc);
}
struct Value *InterpreterDoArrayIdx(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoMemberAccess(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoReturn(struct LittleLangMachine *llm, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoMut(struct LittleLangMachine *llm, struct Ast *ast) {
    unsigned int i;
    struct Ast *names, *values, *curName;
    struct Symbol *symbol;
    struct Value *value;
    names = ast->Children[0];
    values = ast->Children[1];
    for (i = 0; i < names->NumChildren; ++i) {
        curName = names->Children[i];
        if (SymbolTableFindNearest(llm->CurrentScope, curName->u.SymbolName, &symbol)) {
            printf("Symbol already defined: '%s' at %s:%d:%d\n",
                symbol->Key,
                symbol->SrcLoc.Filename,
                symbol->SrcLoc.LineNumber,
                symbol->SrcLoc.ColumnNumber);
            return &g_TheNilValue;
        }
        value = InterpreterRunAst(llm, values->Children[i]);
        SymbolTableInsert(llm->CurrentScope, value, curName->u.SymbolName, 1, curName->SrcLoc);
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoConst(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Ast *name, *valueAst;
    struct Symbol *symbol;
    struct Value *value;
    name = ast->Children[0];
    valueAst = ast->Children[1];
    if (SymbolTableFindNearest(llm->CurrentScope, name->u.SymbolName, &symbol)) {
        printf("Symbol already defined: '%s' at %s:%d:%d\n",
            symbol->Key,
            symbol->SrcLoc.Filename,
            symbol->SrcLoc.LineNumber,
            symbol->SrcLoc.ColumnNumber);
        return &g_TheNilValue;
    }
    value = InterpreterRunAst(llm, valueAst);
    SymbolTableInsert(llm->CurrentScope, value, name->u.SymbolName, 0, name->SrcLoc);
    return &g_TheNilValue;
}
struct Value *InterpreterDoFor(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Ast *pre, *cond, *body, *post;
    struct Value *value = &g_TheNilValue;
    pre = ast->Children[0];
    cond = ast->Children[1];
    body = ast->Children[2];
    post = ast->Children[3];
    SymbolTablePushScope(&(llm->CurrentScope));
    InterpreterRunAst(llm, pre);
    while (1) {
        if (&g_TheTrueValue != InterpreterRunAst(llm, cond)) {
            break;
        }
        value = InterpreterRunAst(llm, body);
        InterpreterRunAst(llm, post);
    }
    SymbolTablePopScope(&(llm->CurrentScope));
    return value;
}
struct Value *InterpreterDoWhile(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Ast *cond, *body;
    struct Value *value = &g_TheNilValue;
    cond = ast->Children[0];
    body = ast->Children[1];
    SymbolTablePushScope(&(llm->CurrentScope));
    while (1) {
        if (&g_TheTrueValue != InterpreterRunAst(llm, cond)) {
            break;
        }
        value = InterpreterRunAst(llm, body);
    }
    SymbolTablePopScope(&(llm->CurrentScope));
    return value;
}
struct Value *InterpreterDoIfElse(struct LittleLangMachine *llm, struct Ast *ast) {
    struct Ast *cond, *body, *ifelse;
    struct Value *value;
    cond = ast->Children[0];
    body = ast->Children[1];
    ifelse = ast->Children[2];
    SymbolTablePushScope(&(llm->CurrentScope));
    if (&g_TheTrueValue == InterpreterRunAst(llm, cond)) {
        value = InterpreterRunAst(llm, body);
    }
    else if (ifelse && IfElseExpr == ifelse->Type){
        value = InterpreterDoIfElse(llm, ifelse);
    }
    else if (ifelse && Body == ifelse->Type) {
        value = InterpreterRunAst(llm, ifelse);
    }
    else {
        value = &g_TheNilValue;
    }
    SymbolTablePopScope(&(llm->CurrentScope));
    return value;
}


/********************* Public Functions **********************/

int InterpreterInit(struct LittleLangMachine *llm) {
    return RegisterRuntimes(llm);
}

int InterpreterRunProgram(struct LittleLangMachine *llm) {
    unsigned int i;
    if (!llm->Program) {
        return R_OK;
    }
    for (i = 0; i < llm->Program->NumChildren; ++i) {
        InterpreterRunAst(llm, llm->Program->Children[i]);
    }
    return R_OK;
}

struct Value *InterpreterRunAst(struct LittleLangMachine *llm, struct Ast *ast) {
    switch (ast->Type) {
        case Body: return InterpreterDoBody(llm, ast);
        
        case BAddExpr: return InterpreterDoAdd(llm, ast);
        case BSubExpr: return InterpreterDoSub(llm, ast);
        case BMulExpr: return InterpreterDoMul(llm, ast);
        case BDivExpr: return InterpreterDoDiv(llm, ast);
        case BModExpr: return InterpreterDoMod(llm, ast);
        case BPowExpr: return InterpreterDoPow(llm, ast);
        case BLShift: return InterpreterDoLShift(llm, ast);
        case BRShift: return InterpreterDoRShift(llm, ast);
        case BArithOrExpr: return InterpreterDoArithOr(llm, ast);
        case BArithAndExpr: return InterpreterDoArithAnd(llm, ast);
        case BArithXorExpr: return InterpreterDoXorExpr(llm, ast);

        case BLogicOrExpr: return InterpreterDoLogicOr(llm, ast);
        case BLogicAndExpr: return InterpreterDoLogicAnd(llm, ast);
        case BLogicEqExpr: return InterpreterDoLogicEq(llm, ast);
        case BLogicNotEqExpr: return InterpreterDoLogicNotEq(llm, ast);
        case BLogicLtExpr: return InterpreterDoLogicLt(llm, ast);
        case BLogicLtEqExpr: return InterpreterDoLogicLtEq(llm, ast);
        case BLogicGtExpr: return InterpreterDoLogicGt(llm, ast);
        case BLogicGtEqExpr: return InterpreterDoLogicGtEq(llm, ast);

        case UNegExpr: return InterpreterDoNegate(llm, ast);
        case ULogicNotExpr: return InterpreterDoLogicNot(llm, ast);

        case AssignExpr: return InterpreterDoAssign(llm, ast);

        case BooleanNode: return InterpreterDoBoolean(ast);
        case RealNode: return InterpreterDoReal(ast);
        case IntegerNode: return InterpreterDoInteger(ast);
        case StringNode: return InterpreterDoString(ast);
        case SymbolNode: return InterpreterDoSymbol(llm, ast);
        case FunctionNode: return InterpreterDoDefFunction(llm, ast);

        case CallExpr: return InterpreterDoCall(llm, ast);
        case ArrayIdxExpr: return InterpreterDoArrayIdx(llm, ast);
        case MemberAccessExpr: return InterpreterDoMemberAccess(llm, ast);
        case ReturnExpr: return InterpreterDoReturn(llm, ast);
        case MutExpr: return InterpreterDoMut(llm, ast);
        case ConstExpr: return InterpreterDoConst(llm, ast);

        case ForExpr: return InterpreterDoFor(llm, ast);
        case WhileExpr: return InterpreterDoWhile(llm, ast);
        case IfElseExpr: return InterpreterDoIfElse(llm, ast);
    }
    return &g_TheNilValue;
}
