#include "interpreter.h"
#include "symbol_table.h"
#include "module_table.h"
#include "globals.h"
#include "runtime/registrar.h"
#include "value.h"
#include "result.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define TO_BOOLEAN(b) ((b) ? &g_TheTrueValue : &g_TheFalseValue)

void at(struct SrcLoc srcLoc) {
    printf(" at %s:%d:%d\n", srcLoc.Filename, srcLoc.LineNumber, srcLoc.ColumnNumber);
}

/* Forward declarations. */
struct Value *InterpreterRunAst(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoBody(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoAdd(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoSub(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoMul(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoDiv(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoMod(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoPow(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLShift(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoRShift(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoArithOr(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoArithAnd(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoXorExpr(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicOr(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicAnd(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicEq(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicNotEq(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicLt(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicLtEq(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicGt(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicGtEq(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoNegate(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoLogicNot(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoAssign(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoSymbol(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoDefFunction(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoCall(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoArrayIdx(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoMemberAccess(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoReturn(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoMut(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoConst(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoFor(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoWhile(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoIfElse(struct Module *module, struct Ast *ast);
struct Value *InterpreterDoBoolean(struct Ast *ast);
struct Value *InterpreterDoReal(struct Ast *ast);
struct Value *InterpreterDoInteger(struct Ast *ast);
struct Value *InterpreterDoString(struct Ast *ast);

/* Function definitions */
struct Value *InterpreterDoBody(struct Module *module, struct Ast *ast) {
    unsigned int i;
    struct Value *value = &g_TheNilValue;
    for (i = 0; i < ast->NumChildren; ++i) {
        value = InterpreterRunAst(module, ast->Children[i]);
    }
    return value;
}
struct Value *InterpreterDoAdd(struct Module *module, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoSub(struct Module *module, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoMul(struct Module *module, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoDiv(struct Module *module, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoMod(struct Module *module, struct Ast *ast) {
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoPow(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoLShift(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoRShift(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoArithOr(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoArithAnd(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoXorExpr(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicOr(struct Module *module, struct Ast *ast) {
    struct Value *lhs, *rhs;
    lhs = InterpreterRunAst(module, ast->Children[0]);
    if (&g_TheTrueValue == lhs) {
        return &g_TheTrueValue;
    }
    rhs = InterpreterRunAst(module, ast->Children[1]);
    if (&g_TheTrueValue == rhs) {
        return &g_TheTrueValue;
    }
    if (&g_TheFalseValue == rhs) {
        return &g_TheFalseValue;
    }
    /* TODO: Runtime error */
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicAnd(struct Module *module, struct Ast *ast) {
    struct Value *lhs, *rhs;
    lhs = InterpreterRunAst(module, ast->Children[0]);
    if (&g_TheFalseValue == lhs) {
        return &g_TheFalseValue;
    }
    rhs = InterpreterRunAst(module, ast->Children[1]);
    if (&g_TheFalseValue == rhs) {
        return &g_TheFalseValue;
    }
    if (&g_TheTrueValue == lhs && lhs == rhs) {
        return &g_TheTrueValue;
    }
    /* TODO: Runtime error */
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicEq(struct Module *module, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoLogicNotEq(struct Module *module, struct Ast *ast) {
    struct Value *value = InterpreterDoLogicEq(module, ast);
    if (&g_TheTrueValue == value) {
        return &g_TheFalseValue;
    }
    else if (&g_TheFalseValue == value) {
        return &g_TheTrueValue;
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicLt(struct Module *module, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoLogicLtEq(struct Module *module, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoLogicGt(struct Module *module, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoLogicGtEq(struct Module *module, struct Ast *ast) {
    int result;
    struct Value *lhs = InterpreterRunAst(module, ast->Children[0]);
    struct Value *rhs = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoNegate(struct Module *module, struct Ast *ast) {
    struct Value *rhs = InterpreterRunAst(module, ast->Children[0]);
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
struct Value *InterpreterDoLogicNot(struct Module *module, struct Ast *ast) {
    struct Value *rhs = InterpreterRunAst(module, ast->Children[0]);
    if (&g_TheTrueValue == rhs) {
        return &g_TheFalseValue;
    }
    if (&g_TheFalseValue == rhs) {
        return &g_TheTrueValue;
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoAssign(struct Module *module, struct Ast *ast) {
    struct Symbol *symbol;
    struct Value *value;
    char *symName = ast->Children[0]->u.SymbolName;
    if (!SymbolTableFindNearest(module->CurrentScope, symName, &symbol)) {
        printf("Trying to assign to undefined symbol: '%s'", symbol->Key);
        at(ast->SrcLoc);
        return &g_TheNilValue;
    }
    if (!symbol->IsMutable) {
        printf("Trying to assign to const symbol: '%s'", symbol->Key);
        at(ast->SrcLoc);
        return &g_TheNilValue;
    }
    value = InterpreterRunAst(module, ast->Children[1]);
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
struct Value *InterpreterDoSymbol(struct Module *module, struct Ast *ast){
    struct Symbol *sym;
    if (SymbolTableFindNearest(module->CurrentScope, ast->u.SymbolName, &sym)) {
        return sym->Value;
    }
    else if (SymbolTableFindLocal(&g_TheGlobalScope, ast->u.SymbolName, &sym)) {
        return sym->Value;
    }
    printf("Undefined symbol: '%s'", ast->u.SymbolName);
    at(ast->SrcLoc);
    return &g_TheNilValue;
}
struct Value *InterpreterDoDefFunction(struct Module *module, struct Ast *ast){
    return &g_TheNilValue;
}
struct Value *InterpreterDoCallBuiltinFn(struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc) {
    struct BuiltinFn *fn = function->v.BuiltinFn;
    if (argc < fn->NumArgs || (argc > fn->NumArgs && !fn->IsVarArgs)) {
        /* TODO: Throw proper error. */
        printf("Wrong number of args for call: '%s', expected '%d' got '%d'",
               fn->Name,
               fn->NumArgs,
               argc);
        at(srcLoc);
        return &g_TheNilValue;
    }
    return function->v.BuiltinFn->Fn(argc, argv);
}
struct Value *InterpreterDoCallFunction(struct Module *module, struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc) {
    unsigned int i;
    struct Value *returnValue, *arg;
    struct Ast *params, *body, *param;
    struct Function *fn = function->v.Function;
    params = fn->Params;
    body = fn->Body;
    if (argc < fn->NumArgs || (argc > fn->NumArgs && !fn->IsVarArgs)) {
        /* TODO: Throw proper error. */
        printf("Wrong number of args for call: '%s', expected '%d' got '%d'",
               fn->Name,
               fn->NumArgs,
               argc);
        at(srcLoc);
        return &g_TheNilValue;
    }
    SymbolTablePushScope(&(module->CurrentScope));
    /* Setup params */
    /* TODO: Handle varargs */
    if (params) {
        for (i = 0; i < params->NumChildren; ++i) {
            arg = argv[i];
            param = params->Children[i];
            SymbolTableInsert(module->CurrentScope, arg, param->u.SymbolName, 1, param->SrcLoc);
        }
    }
    /* Execute body. */
    for (i = 0; i < body->NumChildren; ++i) {
        returnValue = InterpreterRunAst(module, body->Children[i]);
    }
    SymbolTablePopScope(&(module->CurrentScope));
    return returnValue;
}
struct Value *InterpreterDoCall(struct Module *module, struct Ast *ast) {
    struct Value *func = InterpreterRunAst(module, ast->Children[0]);
    unsigned int argc, i;
    struct Value **argv, *arg, *argCopyOrRef;
    struct Ast *args;
    if (&g_TheNilValue == func) {
        return &g_TheNilValue;
    }
    args = ast->Children[1];
    if (args && args->NumChildren > 0) {
        argc = args->NumChildren;
        argv = malloc(sizeof(*argv) * argc);
        for (i = 0; i < argc; ++i) {
            arg = InterpreterRunAst(module, args->Children[i]);
            ValueDuplicate(&argCopyOrRef, arg);
            argv[i] = argCopyOrRef;
        }
    }
    else {
        argc = 0;
        argv = NULL;
    }
    if (func->IsBuiltInFn) {
        return InterpreterDoCallBuiltinFn(func, argc, argv, ast->SrcLoc);
    }
    return InterpreterDoCallFunction(func->v.Function->OwnerModule, func, argc, argv, ast->SrcLoc);
}
struct Value *InterpreterDoArrayIdx(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoMemberAccess(struct Module *module, struct Ast *ast) {
    struct Ast *symbol = ast->Children[0];
    struct Ast *member = ast->Children[1];
    struct Module *import;

    ModuleTableFind(module->Imports, symbol->u.SymbolName, &import);
    if (import) {
        return InterpreterRunAst(import, member);
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoReturn(struct Module *module, struct Ast *ast) {
    return &g_TheNilValue;
}
struct Value *InterpreterDoMut(struct Module *module, struct Ast *ast) {
    unsigned int i;
    struct Ast *names, *values, *curName;
    struct Symbol *symbol;
    struct Value *value;
    names = ast->Children[0];
    values = ast->Children[1];
    for (i = 0; i < names->NumChildren; ++i) {
        curName = names->Children[i];
        if (SymbolTableFindNearest(module->CurrentScope, curName->u.SymbolName, &symbol)) {
            printf("Symbol already defined: '%s' at %s:%d:%d\n",
                symbol->Key,
                symbol->SrcLoc.Filename,
                symbol->SrcLoc.LineNumber,
                symbol->SrcLoc.ColumnNumber);
            return &g_TheNilValue;
        }
        value = InterpreterRunAst(module, values->Children[i]);
        SymbolTableInsert(module->CurrentScope, value, curName->u.SymbolName, 1, curName->SrcLoc);
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoConst(struct Module *module, struct Ast *ast) {
    struct Ast *name, *valueAst;
    struct Symbol *symbol;
    struct Value *value;
    name = ast->Children[0];
    valueAst = ast->Children[1];
    if (SymbolTableFindNearest(module->CurrentScope, name->u.SymbolName, &symbol)) {
        printf("Symbol already defined: '%s' at %s:%d:%d\n",
            symbol->Key,
            symbol->SrcLoc.Filename,
            symbol->SrcLoc.LineNumber,
            symbol->SrcLoc.ColumnNumber);
        return &g_TheNilValue;
    }
    value = InterpreterRunAst(module, valueAst);
    SymbolTableInsert(module->CurrentScope, value, name->u.SymbolName, 0, name->SrcLoc);
    return &g_TheNilValue;
}
struct Value *InterpreterDoFor(struct Module *module, struct Ast *ast) {
    struct Ast *pre, *cond, *body, *post;
    struct Value *value = &g_TheNilValue;
    pre = ast->Children[0];
    cond = ast->Children[1];
    body = ast->Children[2];
    post = ast->Children[3];
    SymbolTablePushScope(&(module->CurrentScope));
    InterpreterRunAst(module, pre);
    while (1) {
        if (&g_TheTrueValue != InterpreterRunAst(module, cond)) {
            break;
        }
        value = InterpreterRunAst(module, body);
        InterpreterRunAst(module, post);
    }
    SymbolTablePopScope(&(module->CurrentScope));
    return value;
}
struct Value *InterpreterDoWhile(struct Module *module, struct Ast *ast) {
    struct Ast *cond, *body;
    struct Value *value = &g_TheNilValue;
    cond = ast->Children[0];
    body = ast->Children[1];
    SymbolTablePushScope(&(module->CurrentScope));
    while (1) {
        if (&g_TheTrueValue != InterpreterRunAst(module, cond)) {
            break;
        }
        value = InterpreterRunAst(module, body);
    }
    SymbolTablePopScope(&(module->CurrentScope));
    return value;
}
struct Value *InterpreterDoIfElse(struct Module *module, struct Ast *ast) {
    struct Ast *cond, *body, *ifelse;
    struct Value *value;
    cond = ast->Children[0];
    body = ast->Children[1];
    ifelse = ast->Children[2];
    SymbolTablePushScope(&(module->CurrentScope));
    if (&g_TheTrueValue == InterpreterRunAst(module, cond)) {
        value = InterpreterRunAst(module, body);
    }
    else if (ifelse && IfElseExpr == ifelse->Type){
        value = InterpreterDoIfElse(module, ifelse);
    }
    else if (ifelse && Body == ifelse->Type) {
        value = InterpreterRunAst(module, ifelse);
    }
    else {
        value = &g_TheNilValue;
    }
    SymbolTablePopScope(&(module->CurrentScope));
    return value;
}


/********************* Public Functions **********************/

int InterpreterInit(void) {
    return RegisterRuntimes();
}

int InterpreterRunProgram(struct Module *module) {
    unsigned int i;
    if (!module->Program) {
        return R_OK;
    }
    for (i = 0; i < module->Program->NumChildren; ++i) {
        InterpreterRunAst(module, module->Program->Children[i]);
    }
    return R_OK;
}

struct Value *InterpreterRunAst(struct Module *module, struct Ast *ast) {
    switch (ast->Type) {
        case Body: return InterpreterDoBody(module, ast);
        
        case BAddExpr: return InterpreterDoAdd(module, ast);
        case BSubExpr: return InterpreterDoSub(module, ast);
        case BMulExpr: return InterpreterDoMul(module, ast);
        case BDivExpr: return InterpreterDoDiv(module, ast);
        case BModExpr: return InterpreterDoMod(module, ast);
        case BPowExpr: return InterpreterDoPow(module, ast);
        case BLShift: return InterpreterDoLShift(module, ast);
        case BRShift: return InterpreterDoRShift(module, ast);
        case BArithOrExpr: return InterpreterDoArithOr(module, ast);
        case BArithAndExpr: return InterpreterDoArithAnd(module, ast);
        case BArithXorExpr: return InterpreterDoXorExpr(module, ast);

        case BLogicOrExpr: return InterpreterDoLogicOr(module, ast);
        case BLogicAndExpr: return InterpreterDoLogicAnd(module, ast);
        case BLogicEqExpr: return InterpreterDoLogicEq(module, ast);
        case BLogicNotEqExpr: return InterpreterDoLogicNotEq(module, ast);
        case BLogicLtExpr: return InterpreterDoLogicLt(module, ast);
        case BLogicLtEqExpr: return InterpreterDoLogicLtEq(module, ast);
        case BLogicGtExpr: return InterpreterDoLogicGt(module, ast);
        case BLogicGtEqExpr: return InterpreterDoLogicGtEq(module, ast);

        case UNegExpr: return InterpreterDoNegate(module, ast);
        case ULogicNotExpr: return InterpreterDoLogicNot(module, ast);

        case AssignExpr: return InterpreterDoAssign(module, ast);

        case BooleanNode: return InterpreterDoBoolean(ast);
        case RealNode: return InterpreterDoReal(ast);
        case IntegerNode: return InterpreterDoInteger(ast);
        case StringNode: return InterpreterDoString(ast);
        case SymbolNode: return InterpreterDoSymbol(module, ast);
        case FunctionNode: return InterpreterDoDefFunction(module, ast);

        case CallExpr: return InterpreterDoCall(module, ast);
        case ArrayIdxExpr: return InterpreterDoArrayIdx(module, ast);
        case MemberAccessExpr: return InterpreterDoMemberAccess(module, ast);
        case ReturnExpr: return InterpreterDoReturn(module, ast);
        case MutExpr: return InterpreterDoMut(module, ast);
        case ConstExpr: return InterpreterDoConst(module, ast);

        case ForExpr: return InterpreterDoFor(module, ast);
        case WhileExpr: return InterpreterDoWhile(module, ast);
        case IfElseExpr: return InterpreterDoIfElse(module, ast);
    }
    return &g_TheNilValue;
}
