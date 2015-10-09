#include "interpreter.h"
#include "symbol_table.h"
#include "module_table.h"
#include "globals.h"
#include "runtime/registrar.h"
#include "runtime/gc.h"
#include "value.h"
#include "result.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TO_BOOLEAN(b) ((b) ? &g_TheTrueValue : &g_TheFalseValue)

#define DEREF_IF_SYMBOL(s) ((s) = (s)->IsSymbol ? (s)->v.Symbol->Value : s)

#define IS_NUMERIC(v) ((v) == TypeReal || (v) == TypeInteger)

void at(struct SrcLoc srcLoc) {
    printf(" at %s:%d:%d\n", srcLoc.Filename, srcLoc.LineNumber, srcLoc.ColumnNumber);
}

static unsigned int NumToInjectIntoNextCall;
static struct Value *InjectIntoNextCall[4];

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
struct Value *InterpreterDoCallBuiltinFn(struct Module *module, struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc);
struct Value *InterpreterDoCallFunction(struct Module *module, struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc);

static inline struct Value *DispatchBinaryOperationMethod(struct Module *module, struct Ast *ast, char *methodName) {
    struct Value *lhs, *rhs, *method;
    struct Value *argv[2];
    /* TODO: maybe these values need preservation */
    lhs = InterpreterRunAst(module, ast->Children[0]);
    DEREF_IF_SYMBOL(lhs);
    rhs = InterpreterRunAst(module, ast->Children[1]);
    DEREF_IF_SYMBOL(rhs);
    TypeInfoLookupMethod(lhs->TypeInfo, methodName, &method);
    if (!method) {
        printf("Binary method '%s' not implemented for type of '%s'",
               methodName,
               lhs->TypeInfo->TypeName);
        at(ast->SrcLoc);
        return &g_TheNilValue;
    }
    argv[0] = lhs;
    argv[1] = rhs;
    if (method->IsBuiltInFn) {
        return InterpreterDoCallBuiltinFn(module, method, 2, argv, ast->SrcLoc);
    }
    return InterpreterDoCallFunction(module, method, 2, argv, ast->SrcLoc);
}

static inline struct Value *DispatchPrefixUnaryOperationMethod(struct Module *module, struct Ast *ast, char *methodName) {
    struct Value *rhs, *method;
    struct Value *argv[1];
    rhs = InterpreterRunAst(module, ast->Children[0]);
    DEREF_IF_SYMBOL(rhs);
    TypeInfoLookupMethod(rhs->TypeInfo, methodName, &method);
    if (!method) {
        printf("Prefix unary method '%s' not implemented for type of '%s'",
               methodName,
               rhs->TypeInfo->TypeName);
        at(ast->SrcLoc);
        return &g_TheNilValue;
    }
    argv[0] = rhs;
    if (method->IsBuiltInFn) {
        return InterpreterDoCallBuiltinFn(module, method, 1, argv, ast->SrcLoc);
    }
    return InterpreterDoCallFunction(module, method, 1, argv, ast->SrcLoc);
}

struct Value *InterpreterDispatchMethod(struct Module *module, struct Value *object, char *methodName, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc) {
    struct Value *method, *result;
    unsigned int i, newArgc = argc + 1;
    struct Value **newArgv;

    DEREF_IF_SYMBOL(object);
    TypeInfoLookupMethod(object->TypeInfo, methodName, &method);
    if (!method) {
        printf("Method '%s' not implemented for type of '%s'",
               methodName,
               object->TypeInfo->TypeName);
        at(srcLoc);
        return &g_TheNilValue;
    }

    newArgv = malloc(sizeof(*newArgv) * newArgc);
    newArgv[0] = object;
    for (i = 0; i < argc; ++i) {
        newArgv[i+1] = argv[i];
    }
    if (method->IsBuiltInFn) {
        result = InterpreterDoCallBuiltinFn(module, method, newArgc, newArgv, srcLoc);
    }
    else {
        result = InterpreterDoCallFunction(module, method, newArgc, newArgv, srcLoc);
    }
    free(newArgv);
    return result;
}

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
    return DispatchBinaryOperationMethod(module, ast, "__add__");
}
struct Value *InterpreterDoSub(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__sub__");
}
struct Value *InterpreterDoMul(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__mul__");
}
struct Value *InterpreterDoDiv(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__div__");
}
struct Value *InterpreterDoMod(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__mod__");
}
struct Value *InterpreterDoPow(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__pow__");
}
struct Value *InterpreterDoLShift(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__lshift__");
}
struct Value *InterpreterDoRShift(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__rshift__");
}
struct Value *InterpreterDoArithOr(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__or__");
}
struct Value *InterpreterDoArithAnd(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__and__");
}
struct Value *InterpreterDoXorExpr(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__xor__");
}
struct Value *InterpreterDoLogicOr(struct Module *module, struct Ast *ast) {
    struct Value *lhs, *rhs;
    lhs = InterpreterRunAst(module, ast->Children[0]);
    DEREF_IF_SYMBOL(lhs);
    if (&g_TheTrueValue == lhs) {
        return &g_TheTrueValue;
    }
    rhs = InterpreterRunAst(module, ast->Children[1]);
    DEREF_IF_SYMBOL(rhs);
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
    DEREF_IF_SYMBOL(lhs);
    if (&g_TheFalseValue == lhs) {
        return &g_TheFalseValue;
    }
    rhs = InterpreterRunAst(module, ast->Children[1]);
    DEREF_IF_SYMBOL(rhs);
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
    return DispatchBinaryOperationMethod(module, ast, "__eq__");
}
struct Value *InterpreterDoLogicNotEq(struct Module *module, struct Ast *ast) {
    struct Value *value = InterpreterDoLogicEq(module, ast);
    DEREF_IF_SYMBOL(value);
    if (&g_TheTrueValue == value) {
        return &g_TheFalseValue;
    }
    else if (&g_TheFalseValue == value) {
        return &g_TheTrueValue;
    }
    return &g_TheNilValue;
}
struct Value *InterpreterDoLogicLt(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__lt__");
}
struct Value *InterpreterDoLogicLtEq(struct Module *module, struct Ast *ast) {
    struct Value *eq, *lt = InterpreterDoLogicLt(module, ast);
    if (&g_TheTrueValue == lt) {
        return &g_TheTrueValue;
    }
    eq = InterpreterDoLogicEq(module, ast);
    if (&g_TheTrueValue == eq) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
struct Value *InterpreterDoLogicGt(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__gt__");
}
struct Value *InterpreterDoLogicGtEq(struct Module *module, struct Ast *ast) {
    struct Value *eq, *gt = InterpreterDoLogicGt(module, ast);
    if (&g_TheTrueValue == gt) {
        return &g_TheTrueValue;
    }
    eq = InterpreterDoLogicEq(module, ast);
    if (&g_TheTrueValue == eq) {
        return &g_TheTrueValue;
    }
    return &g_TheFalseValue;
}
struct Value *InterpreterDoNegate(struct Module *module, struct Ast *ast) {
    return DispatchPrefixUnaryOperationMethod(module, ast, "__neg__");
}
struct Value *InterpreterDoLogicNot(struct Module *module, struct Ast *ast) {
    return DispatchPrefixUnaryOperationMethod(module, ast, "__not__");
}
struct Value *InterpreterDoAssign(struct Module *module, struct Ast *ast) {
    struct Symbol *symbol;
    struct Value *rvalue, *lvalue;
    lvalue = InterpreterRunAst(module, ast->Children[0]);
    rvalue = InterpreterRunAst(module, ast->Children[1]);
    DEREF_IF_SYMBOL(rvalue);
    if (lvalue->IsSymbol) {
        symbol = lvalue->v.Symbol;
        if (!symbol->IsMutable) {
            printf("Trying to assign to const symbol: '%s'", symbol->Key);
            at(ast->SrcLoc);
            return &g_TheNilValue;
        }
        symbol->Value = rvalue;
        return symbol->Value;
    }
    return &g_TheNilValue;
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
    struct Value *value = ValueAlloc();
    value->IsSymbol = 1;
    if (SymbolTableFindNearest(module->CurrentScope, ast->u.SymbolName, &sym)) {
        value->v.Symbol = sym;
        return value;
    }
    else if (SymbolTableFindLocal(g_TheGlobalScope, ast->u.SymbolName, &sym)) {
        value->v.Symbol = sym;
        return value;
    }
    printf("Undefined symbol: '%s'", ast->u.SymbolName);
    at(ast->SrcLoc);
    free(value);
    return &g_TheNilValue;
}
struct Value *InterpreterDoDefFunction(struct Module *module, struct Ast *ast){
    return &g_TheNilValue;
}
struct Value *InterpreterDoCallBuiltinFn(struct Module *module, struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc) {
    struct Value *value;
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
    value = function->v.BuiltinFn->Fn(module, argc, argv);
    return value;
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
        DEREF_IF_SYMBOL(returnValue);
    }
    ValueDuplicate(&returnValue, returnValue);
    SymbolTablePopScope(&(module->CurrentScope));
    SymbolTableAssign(module->CurrentScope, returnValue, "#_return_#", 1, srcLoc);
    return returnValue;
}
struct Value *InterpreterDoCall(struct Module *module, struct Ast *ast) {
    struct Value *func = InterpreterRunAst(module, ast->Children[0]);
    unsigned int argc, i, argvIdx;
    struct Value **argv, *arg, *argCopyOrRef, *ret;
    struct Ast *args;
    DEREF_IF_SYMBOL(func);
    if (&g_TheNilValue == func) {
        return &g_TheNilValue;
    }
    args = ast->Children[1];
    argc = 0;
    if (args) {
        argc += args->NumChildren;
    }
    argc += NumToInjectIntoNextCall;
    argv = malloc(sizeof(*argv) * argc);
    argvIdx = 0;
    if (NumToInjectIntoNextCall > 0) {
        for (; argvIdx < NumToInjectIntoNextCall; ++argvIdx) {
            argv[argvIdx] = InjectIntoNextCall[argvIdx];
        }
    }
    if (args) {
        for (i = 0; i < args->NumChildren; ++i, ++argvIdx) {
            arg = InterpreterRunAst(module, args->Children[i]);
            DEREF_IF_SYMBOL(arg);
            ValueDuplicate(&argCopyOrRef, arg);
            argv[argvIdx] = argCopyOrRef;
        }
    }
    NumToInjectIntoNextCall = 0;
    if (func->IsBuiltInFn) {
        ret = InterpreterDoCallBuiltinFn(module, func, argc, argv, ast->SrcLoc);
    }
    else {
        ret = InterpreterDoCallFunction(func->v.Function->OwnerModule, func, argc, argv, ast->SrcLoc);
    }
    GC_Collect();
    free(argv);
    return ret;
}
struct Value *InterpreterDoArrayIdx(struct Module *module, struct Ast *ast) {
    return DispatchBinaryOperationMethod(module, ast, "__index__");
}
struct Value *InterpreterDoMemberAccess(struct Module *module, struct Ast *ast) {
    struct Ast *left = ast->Children[0];
    struct Ast *memberAst = ast->Children[1];
    struct Value *value, *member;
    struct Module *import;
    struct TypeInfo *typeInfo;

    if (SymbolNode == left->Type) {
        ModuleTableFind(module->Imports, left->u.SymbolName, &import);
        if (import) {
            return InterpreterRunAst(import, memberAst);
        }

        TypeTableFind(&g_TheGlobalTypeTable, left->u.SymbolName, &typeInfo);
        if (typeInfo) {
            TypeInfoLookupMethod(typeInfo, memberAst->u.SymbolName, &member);
            if (member) {
                return member;
            }
            printf("Type '%s' does not implement '%s'",
                typeInfo->TypeName,
                memberAst->u.SymbolName);
            at(ast->SrcLoc);
            return &g_TheNilValue;
        }
    }
    
    value = InterpreterRunAst(module, left);
    DEREF_IF_SYMBOL(value);
    TypeInfoLookupMethod(value->TypeInfo, memberAst->u.SymbolName, &member);
    if (member) {
        NumToInjectIntoNextCall = 1;
        InjectIntoNextCall[0] = value;
        return member;
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

    if (values && values->NumChildren > names->NumChildren) {
        printf("Too many values in 'mut' statement");
        at(ast->SrcLoc);
        return &g_TheNilValue;
    }

    for (i = 0; i < names->NumChildren; ++i) {
        curName = names->Children[i];
        if (SymbolTableFindNearest(module->CurrentScope, curName->u.SymbolName, &symbol)) {
            printf("Symbol already defined: '%s'", symbol->Key);
            at(symbol->SrcLoc);
            return &g_TheNilValue;
        }
        if (!values || i >= values->NumChildren) {
            value = &g_TheNilValue;
        }
        else {
            value = InterpreterRunAst(module, values->Children[i]);
            DEREF_IF_SYMBOL(value);
        }
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
        printf("Symbol already defined: '%s'", symbol->Key);
        at(symbol->SrcLoc);
        return &g_TheNilValue;
    }
    value = InterpreterRunAst(module, valueAst);
    DEREF_IF_SYMBOL(value);
    SymbolTableInsert(module->CurrentScope, value, name->u.SymbolName, 0, name->SrcLoc);
    return &g_TheNilValue;
}
struct Value *InterpreterDoFor(struct Module *module, struct Ast *ast) {
    struct Ast *pre, *cond, *body, *post;
    struct Value *c;
    pre = ast->Children[0];
    cond = ast->Children[1];
    body = ast->Children[2];
    post = ast->Children[3];
    SymbolTablePushScope(&(module->CurrentScope));
    InterpreterRunAst(module, pre);
    while (1) {
        c = InterpreterRunAst(module, cond);
        DEREF_IF_SYMBOL(c);
        if (&g_TheTrueValue != c) {
            break;
        }
        InterpreterRunAst(module, body);
        InterpreterRunAst(module, post);
    }
    SymbolTablePopScope(&(module->CurrentScope));
    return &g_TheNilValue;
}
struct Value *InterpreterDoWhile(struct Module *module, struct Ast *ast) {
    struct Ast *cond, *body;
    struct Value *c;
    cond = ast->Children[0];
    body = ast->Children[1];
    SymbolTablePushScope(&(module->CurrentScope));
    while (1) {
        c = InterpreterRunAst(module, cond);
        DEREF_IF_SYMBOL(c);
        if (&g_TheTrueValue != c) {
            break;
        }
        InterpreterRunAst(module, body);
    }
    SymbolTablePopScope(&(module->CurrentScope));
    return &g_TheNilValue;
}
struct Value *InterpreterDoIfElse(struct Module *module, struct Ast *ast) {
    struct Ast *cond, *body, *ifelse;
    struct Value *value;
    cond = ast->Children[0];
    body = ast->Children[1];
    ifelse = ast->Children[2];
    SymbolTablePushScope(&(module->CurrentScope));
    value = InterpreterRunAst(module, cond);
    DEREF_IF_SYMBOL(value);
    if (&g_TheTrueValue == value) {
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
    GC_RegisterSymbolTable(module->ModuleScope); /* TODO: Handle return */
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
