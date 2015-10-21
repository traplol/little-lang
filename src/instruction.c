#include "instruction.h"
#include "globals.h"
#include "value.h"
#include "llstring.h"

#include "helpers/macro_helpers.h"
#include "helpers/strings.h"
#include "result.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int locals = 0;
static int FPOffset = 0;
static unsigned int isInFunctionDef = 0;

/****************************** Symbol stack ********************************/

struct Sym {
    char *Key;
    int Idx;
};

static struct SymStack {
    int Length;
    int Capacity;
    struct Sym **Stack;
} symStack;

static void symStackInit(void) {
    if (symStack.Capacity != 0) {
        return;
    }
    symStack.Length = 0;
    symStack.Capacity = 256;
    symStack.Stack = calloc(sizeof *symStack.Stack, symStack.Capacity);
}

static void symStackResize(void) {
    int i;
    struct Sym **newStack;
    symStack.Capacity *= 2;
    newStack = calloc(sizeof *symStack.Stack, symStack.Capacity);
    for (i = 0; i < symStack.Length; ++i) {
        newStack[i] = symStack.Stack[i];
    }
    free(symStack.Stack);
    symStack.Stack = newStack;
}
static struct Sym *symStackGetIdx(char *key) {
    int i;
    if (!key) {
        return NULL;
    }
    for (i = symStack.Length - 1; i >= 0; --i) {
        if (!strcmp(key, symStack.Stack[i]->Key)) {
            return symStack.Stack[i];
        }
    }
    return NULL;
}
static struct Sym *symStackPush(char *key) {
    struct Sym *s;
    if (!key) {
        return NULL;
    }
    if (symStack.Length + 1 >= symStack.Capacity) {
        symStackResize();
    }
    s = malloc(sizeof *s);
    s->Key = strdup(key);
    s->Idx = -(symStack.Length - FPOffset + 1);
    symStack.Stack[symStack.Length] = s;
    symStack.Length++;
    return s;
}
static void symStackPopN(int n) {
    for (;n > 0; --n) {
        free(symStack.Stack[symStack.Length--]);
    }
}
static void symStackPop(void) {
    symStackPopN(1);
}

/**************************** Symbol stack end ******************************/


#define IT_TO_S(it) (STR(it)+STRLEN_LIT("INS_"))
#define INS_STRUCT(it, numOps) [it] = {.AsString = IT_TO_S(it), .NumOperands = numOps}

static const struct {
    const char *AsString;
    const int NumOperands;
} InstructionTypes[INS_NUM_INSTRUCTION_TYPES] = {
    INS_STRUCT(INS_binary_add, 0),
    INS_STRUCT(INS_binary_sub, 0),
    INS_STRUCT(INS_binary_mul, 0),
    INS_STRUCT(INS_binary_div, 0),
    INS_STRUCT(INS_binary_mod, 0),
    INS_STRUCT(INS_binary_pow, 0),
    INS_STRUCT(INS_binary_lshift, 0),
    INS_STRUCT(INS_binary_rshift, 0),
    INS_STRUCT(INS_binary_or, 0),
    INS_STRUCT(INS_binary_and, 0),
    INS_STRUCT(INS_binary_xor, 0),
    INS_STRUCT(INS_binary_cmp_eq, 0),
    INS_STRUCT(INS_binary_cmp_neq, 0),
    INS_STRUCT(INS_binary_cmp_lt, 0),
    INS_STRUCT(INS_binary_cmp_lte, 0),
    INS_STRUCT(INS_binary_cmp_gt, 0),
    INS_STRUCT(INS_binary_cmp_gte, 0),

    INS_STRUCT(INS_unary_negative, 0),
    INS_STRUCT(INS_unary_not, 0),

    INS_STRUCT(INS_push, 1),
    INS_STRUCT(INS_pop, 1),

    INS_STRUCT(INS_jmp_if_false, 1),
    INS_STRUCT(INS_jmp, 1),
    INS_STRUCT(INS_return, 0),
    INS_STRUCT(INS_call, 1),

    INS_STRUCT(INS_get_member, 0),

    INS_STRUCT(INS_assign_to, 1),
    INS_STRUCT(INS_set_local_mut, 1),
    INS_STRUCT(INS_set_global_mut, 1),
    INS_STRUCT(INS_set_local_mut_nil, 1),
    INS_STRUCT(INS_set_global_mut_nil, 1),

    INS_STRUCT(INS_set_local_const, 1),
    INS_STRUCT(INS_set_global_const, 1)
};

static const enum InstructionType AstNodeToInstructionType[] = {
    [BAddExpr] = INS_binary_add,
    [BSubExpr] = INS_binary_sub,
    [BMulExpr] = INS_binary_mul,
    [BDivExpr] = INS_binary_div,
    [BModExpr] = INS_binary_mod,
    [BPowExpr] = INS_binary_pow,
    [BLShift] = INS_binary_lshift,
    [BRShift] = INS_binary_rshift,
    [BArithOrExpr] = INS_binary_or,
    [BArithAndExpr] = INS_binary_and,
    [BArithXorExpr] = INS_binary_xor,

    [BLogicEqExpr] = INS_binary_cmp_eq,
    [BLogicNotEqExpr] = INS_binary_cmp_neq,
    [BLogicLtExpr] = INS_binary_cmp_lt,
    [BLogicLtEqExpr] = INS_binary_cmp_lte,
    [BLogicGtExpr] = INS_binary_cmp_gt,
    [BLogicGtEqExpr] = INS_binary_cmp_gte,

    [UNegExpr] = INS_unary_negative,
    [ULogicNotExpr] = INS_unary_not,
};

static int isOperation(struct Ast *node) {
    enum AstNodeType t = node->Type;

    return NilNode != t &&
        BooleanNode != t &&
        RealNode != t &&
        IntegerNode != t &&
        StringNode != t &&
        SymbolNode != t;
}

static int isTerm(struct Ast *node) {
    return !isOperation(node);
}

static struct Operand *OperandAlloc(void) {
    struct Operand *operand = calloc(sizeof *operand, 1);
    return operand;
}

static const char *RegisterToString(enum Register reg) {
    switch (reg) {
        case REG_FramePointer:
            return "$fp";
    }
}

static inline char *OperandLabelToString(struct Operand *operand) {
    return strdup(operand->u.LabelName);
}
static inline char *OperandRegisterToString(struct Operand *operand) {
    return strdup(RegisterToString(operand->u.Register));
}
static inline char *OperandValueOfRegisterWithOffsetToString(struct Operand *operand) {
    char buf[80];
    const char *reg = RegisterToString(operand->u.ValueOfRegisterWithOffset.Register);
    snprintf(buf, 79, "%d(%s)",
             operand->u.ValueOfRegisterWithOffset.Offset,
             reg);
    return strdup(buf);
}
static inline char *OperandSymbolToString(struct Operand *operand) {
    return strdup(operand->u.SymbolName);
}
static inline char *OperandIntegerToString(struct Operand *operand) {
    char buf[80];
    snprintf(buf, 79, "%d", operand->u.Integer);
    return strdup(buf);
}
static inline char *OperandRealToString(struct Operand *operand) {
    char buf[80];
    snprintf(buf, 79, "%f", operand->u.Real);
    return strdup(buf);
}
static inline char *OperandStringToString(struct Operand *operand) {
    return strdup(operand->u.String->CString);
}

static char *OperandToString(struct Operand *operand) {
    if (!operand) {
        return NULL;
    }
    switch (operand->Type) {
        case OP_Label:
            return OperandLabelToString(operand);
        case OP_Register:
            return OperandRegisterToString(operand);
        case OP_ValueOfRegisterWithOffset:
            return OperandValueOfRegisterWithOffsetToString(operand);
        case OP_nil:
            return strdup("nil");
        case OP_true:
            return strdup("true");
        case OP_false:
            return strdup("false");
        case OP_Symbol:
            return OperandSymbolToString(operand);
        case OP_Integer:
            return OperandIntegerToString(operand);
        case OP_Real:
            return OperandRealToString(operand);
        case OP_String:
            return OperandStringToString(operand);
    }
    return NULL;
}

static struct Operand *OperandMakeNil(void) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_nil;
    return o;
}
static struct Operand *OperandMakeBoolean(struct Ast *node) {
    struct Operand *o = OperandAlloc();
    if (&g_TheFalseValue == node->u.Value) {
        o->Type = OP_false;
    }
    else {
        o->Type = OP_true;
    }
    return o;
}
static struct Operand *OperandMakeIntegerC(int i) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_Integer;
    o->u.Integer = i;
    return o;
}
static struct Operand *OperandMakeInteger(struct Ast *node) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_Integer;
    o->u.Integer = node->u.Value->v.Integer;
    return o;
}
static struct Operand *OperandMakeReal(struct Ast *node) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_Real;
    o->u.Real = node->u.Value->v.Real;
    return o;
}
static struct Operand *OperandMakeString(struct Ast *node) {
    struct Operand *o = OperandAlloc();
    char *tmp;
    o->Type = OP_String;
    tmp = strdup(node->u.Value->v.String->CString);
    o->u.String = calloc(sizeof *o->u.String, 1);
    LLStringMake(o->u.String, tmp);
    free(tmp);
    return o;
}
static struct Operand *OperandMakeValueOfRegiserWithOffset(enum Register reg, int offset);
static struct Operand *OperandMakeSymbol(struct Ast *node) {
    struct Operand *o;
    struct Sym *sym = symStackGetIdx(node->u.SymbolName);
    if (!sym || !isInFunctionDef) {
        o = OperandAlloc();
        o->Type = OP_Symbol;
        o->u.SymbolName = strdup(node->u.SymbolName);
        return o;
    }
    else {
        return OperandMakeValueOfRegiserWithOffset(REG_FramePointer, sym->Idx);
    }
}
static struct Operand *OperandMakeLabel(char *labelName) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_Symbol;
    o->u.LabelName = strdup(labelName);
    return o;
}
static struct Operand *OperandMakeRegiser(enum Register reg) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_Register;
    o->u.Register = reg;
    return o;
}
static struct Operand *OperandMakeValueOfRegiserWithOffset(enum Register reg, int offset) {
    struct Operand *o = OperandAlloc();
    o->Type = OP_ValueOfRegisterWithOffset;
    o->u.ValueOfRegisterWithOffset.Register = reg;
    o->u.ValueOfRegisterWithOffset.Offset = offset;
    return o;
}

static struct Operand *OperandMakeConstant(struct Ast *node) {
    if (!node) {
        return NULL;
    }
    switch (node->Type) {
        default: return NULL;
        case NilNode:
            return OperandMakeNil();
        case BooleanNode:
            return OperandMakeBoolean(node);
        case RealNode:
            return OperandMakeReal(node);
        case IntegerNode:
            return OperandMakeInteger(node);
        case StringNode:
            return OperandMakeString(node);
        case SymbolNode:
            return OperandMakeSymbol(node);
    }
}

static struct Instruction *InstructionAlloc(enum InstructionType ins) {
    struct Instruction *ret = calloc(sizeof *ret, 1);
    ret->Instruction = ins;
    ret->NumOperands = InstructionTypeNumOperands(ins);
    ret->Operands = calloc(sizeof *ret->Operands, ret->NumOperands);
    return ret;
}
static struct Instruction *InstructionAllocWithSingleOperand(enum InstructionType ins, struct Operand *operand) {
    struct Instruction *instruction;
    if (InstructionTypeNumOperands(ins) != 1) {
        return NULL;
    }
    instruction = InstructionAlloc(ins);
    instruction->Operands[0] = operand;
    return instruction;
}
struct FlattenedAst *FlattenedAstAlloc(unsigned int capacity) {
    struct FlattenedAst *fast;
    if (capacity == 0) {
        capacity = 4;
    }
    fast = calloc(sizeof *fast, 1);
    fast->Capacity = capacity;
    fast->Nodes = calloc(sizeof *fast->Nodes, fast->Capacity);
    return fast;
}

static struct FlattenedAstNode *FlattenedAstNodeAllocInstruction(struct Instruction *ins) {
    struct FlattenedAstNode *fnode = calloc(sizeof *fnode, 1);
    fnode->IsInstruction = 1;
    fnode->u.Instruction = ins;
    return fnode;
}
static struct FlattenedAstNode *FlattenedAstNodeAllocLabel(char *labelName) {
    struct FlattenedAstNode *fnode = calloc(sizeof *fnode, 1);
    fnode->IsInstruction = 0;
    fnode->u.LabelName = strdup(labelName);
    return fnode;
}

#define FAN_ALLOC_NO_OPS(type) FlattenedAstNodeAllocInstruction(InstructionAlloc(type));
#define FAN_APPEND_INST(fast, ins) FlattenedAstAppend(fast, FlattenedAstNodeAllocInstruction(ins));
static int FlattenedAstAppend(struct FlattenedAst *fast, struct FlattenedAstNode *node);
static void InstructionDispatchGen(struct FlattenedAst *fast, struct Ast *node);

static void FlattenedAstAppendLabel(struct FlattenedAst *fast, char *labelFmt, ...) {
    /* TODO: Resizable buffer. */
    struct FlattenedAstNode *label;
    char name[512];
    va_list args;
    va_start(args, labelFmt);
    vsnprintf(name, 511, labelFmt, args);
    va_end(args);
    label = FlattenedAstNodeAllocLabel(name);
    FlattenedAstAppend(fast, label);
}

static void FlattenedAstGenInstructionWithLabelOperand(struct FlattenedAst *fast, enum InstructionType insType, char *labelFmt, ...) {
    /* TODO: Resizable buffer. */
    struct Instruction *ins;
    struct Operand *label;
    char name[512];
    va_list args;
    va_start(args, labelFmt);
    vsnprintf(name, 511, labelFmt, args);
    va_end(args);
    /* TODO: Validate label and ins */
    label = OperandMakeLabel(name);
    ins = InstructionAllocWithSingleOperand(insType, label);
    FAN_APPEND_INST(fast, ins);
}
static inline void FlattenedAstGenSingleOperandInstruction(struct FlattenedAst *fast, enum InstructionType insType, struct Ast *node) {
    struct Instruction *ins = InstructionAlloc(insType);
    /* TODO: Validate operand */
    struct Operand *operand = OperandMakeConstant(node);
    ins->Operands[0] = operand;
    FAN_APPEND_INST(fast, ins);
}
static inline void FlattenedAstGenNoOperandInstruction(struct FlattenedAst *fast, enum InstructionType insType) {
    struct FlattenedAstNode *instruction = FAN_ALLOC_NO_OPS(insType);
    FlattenedAstAppend(fast, instruction);
}
static void InstructionGenPushFP(struct FlattenedAst *fast) {
    struct Instruction *ins;
    ins = InstructionAlloc(INS_push);
    ins->Operands[0] = OperandMakeRegiser(REG_FramePointer);
    FAN_APPEND_INST(fast, ins);
}
static void InstructionTryGenPush(struct FlattenedAst *fast, struct Ast *node) {
    struct Instruction *ins;
    if (isTerm(node)) {
        ins = InstructionAlloc(INS_push);
        ins->Operands[0] = OperandMakeConstant(node);
        FAN_APPEND_INST(fast, ins);
    }
    else {
        return InstructionDispatchGen(fast, node);
    }
}
static void InstructionGenBody(struct FlattenedAst *fast, struct Ast *node) {
    unsigned int i;
    for (i = 0; i < node->NumChildren; ++i) {
        InstructionTryGenPush(fast, node->Children[i]);
    }
}
static void InstructionGenBinaryExpr(struct FlattenedAst *fast, struct Ast *node) {
    enum InstructionType type = AstNodeToInstructionType[node->Type];
    InstructionTryGenPush(fast, node->Children[0]);
    InstructionTryGenPush(fast, node->Children[1]);
    FlattenedAstGenNoOperandInstruction(fast, type);
}
static void InstructionGenLogicalOrExpr(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenLogicalAndExpr(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenAssign(struct FlattenedAst *fast, struct Ast *node) {
    InstructionTryGenPush(fast, node->Children[1]);
    FlattenedAstGenSingleOperandInstruction(fast, INS_assign_to, node->Children[0]);
}
static void InstructionGenUnaryExpr(struct FlattenedAst *fast, struct Ast *node) {
    enum InstructionType type = AstNodeToInstructionType[node->Type];
    InstructionTryGenPush(fast, node->Children[0]);
    FlattenedAstGenNoOperandInstruction(fast, type);
}
static void InstructionGenImport(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenClass(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenFunction(struct FlattenedAst *fast, struct Ast *node) {
    unsigned int i;
    struct Ast *body, *params;
    int oldLocals = locals;
    isInFunctionDef = 1;

    locals += node->u.Value->v.Function->NumArgs;
    params = node->u.Value->v.Function->Params;

    FPOffset += params->NumChildren;
    FPOffset++; /* return */
    FPOffset++; /* old FP */
    for (i = 0; i < params->NumChildren; ++i) {
        symStackPush(params->Children[i]->u.SymbolName);
    }
    /* undo offset of return and old FP for locals since they are a positive 
       offset from ($fp) */
    FPOffset-=2; 
    
    FlattenedAstAppendLabel(fast, "PROC_%s", node->u.Value->v.Function->Name);
    InstructionGenPushFP(fast);

    body = node->u.Value->v.Function->Body;
    InstructionDispatchGen(fast, body);
    FlattenedAstGenNoOperandInstruction(fast, INS_return);
    symStackPopN(locals-oldLocals);
    locals = oldLocals;
    isInFunctionDef = 0;
}
static void InstructionGenArgs(struct FlattenedAst *fast, struct Ast *args) {
    unsigned int i;
    if (!args) {
        return;
    }
    for (i = 0; i < args->NumChildren; ++i) {
        InstructionTryGenPush(fast, args->Children[i]);
    }
}
static void InstructionGenCall(struct FlattenedAst *fast, struct Ast *node) {
    struct Ast *primary = node->Children[0];
    struct Ast *args = node->Children[1];
    struct Operand *operand;
    struct Instruction *instruction;
    InstructionGenArgs(fast, args);
    InstructionTryGenPush(fast, primary);
    operand = OperandMakeIntegerC(args->NumChildren);
    instruction = InstructionAllocWithSingleOperand(INS_call, operand);
    FAN_APPEND_INST(fast, instruction);
}
static void InstructionGenArrayIdx(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenMemberAccess(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenReturn(struct FlattenedAst *fast, struct Ast *node) {
    
}
static void InstructionGenContinue(struct FlattenedAst *fast) {
    
}
static void InstructionGenBreak(struct FlattenedAst *fast) {
    
}

inline static void InstructionGenSetMut(struct FlattenedAst *fast, struct Ast *node) {
    enum InstructionType which = isInFunctionDef ?
        INS_set_local_mut : INS_set_global_mut;
    FlattenedAstGenSingleOperandInstruction(fast, which, node);
}
inline static void InstructionGenSetMutToNil(struct FlattenedAst *fast, struct Ast *node) {
    enum InstructionType which = isInFunctionDef ?
        INS_set_local_mut_nil : INS_set_global_mut_nil;
    FlattenedAstGenSingleOperandInstruction(fast, which, node);
}
static void InstructionGenMut(struct FlattenedAst *fast, struct Ast *node) {
    unsigned int i;
    struct Ast *symbols = node->Children[0];
    struct Ast *values = node->Children[1];
    if (!values) {
        for (i = 0; i < symbols->NumChildren; ++i, ++locals) {
            symStackPush(symbols->Children[i]->u.SymbolName);
            InstructionGenSetMutToNil(fast, symbols->Children[i]);
        }
    }
    else {
        for (i = 0; i < values->NumChildren; ++i, ++locals) {
            symStackPush(symbols->Children[i]->u.SymbolName);
            InstructionTryGenPush(fast, values->Children[i]);
            InstructionGenSetMut(fast, symbols->Children[i]);
        }
        for (; i < symbols->NumChildren; ++i, ++locals) {
            symStackPush(symbols->Children[i]->u.SymbolName);
            InstructionGenSetMutToNil(fast, symbols->Children[i]);
        }
    }
}
inline static void InstructionGenSetConst(struct FlattenedAst *fast, struct Ast *node) {
    enum InstructionType which = isInFunctionDef ?
        INS_set_local_const : INS_set_global_const;
    FlattenedAstGenSingleOperandInstruction(fast, which, node);
}
static void InstructionGenConst(struct FlattenedAst *fast, struct Ast *node) {
    symStackPush(node->Children[1]->u.SymbolName);
    InstructionGenSetConst(fast, node->Children[0]);
}
static void InstructionGenFor(struct FlattenedAst *fast, struct Ast *node) {
    static unsigned int forCondLabelId = 0, forEndLabelId = 0, forPostLabelId = 0; 
    unsigned int _forCondLabelId, _forEndLabelId, _forPostLabelId;
    InstructionDispatchGen(fast, node->Children[0]); /* pre */
    FlattenedAstAppendLabel(fast, "for_cond_%d", ++forCondLabelId);
    _forCondLabelId = forCondLabelId;

    InstructionTryGenPush(fast, node->Children[1]); /* cond */
    /* Check if the condition passes */
    FlattenedAstGenInstructionWithLabelOperand(fast, INS_jmp_if_false, "for_end_%d", ++forEndLabelId);
    _forEndLabelId = forEndLabelId;

    InstructionGenBody(fast, node->Children[2]); /* body */

    /* We need the for_post_x label for `continue' */
    FlattenedAstAppendLabel(fast, "for_post_%d", ++forPostLabelId);
    _forPostLabelId = forPostLabelId;

    InstructionDispatchGen(fast, node->Children[3]); /* post */
    FlattenedAstGenInstructionWithLabelOperand(fast, INS_jmp, "for_cond_%d", _forCondLabelId);
    FlattenedAstAppendLabel(fast, "for_end_%d", _forEndLabelId);
}
static void InstructionGenWhile(struct FlattenedAst *fast, struct Ast *node) {
    static unsigned int whileCondLabelId = 0, whileEndLabelId = 0;
    unsigned int _whileCondLabelId, _whileEndLabelId;
    FlattenedAstAppendLabel(fast, "while_cond_%d", ++whileCondLabelId);
    _whileCondLabelId = whileCondLabelId;
    InstructionTryGenPush(fast, node->Children[0]); /* cond */
    FlattenedAstGenInstructionWithLabelOperand(fast, INS_jmp_if_false, "while_end_%d", ++whileEndLabelId);
    _whileEndLabelId = whileEndLabelId;

    InstructionGenBody(fast, node->Children[1]); /* body */
    FlattenedAstGenInstructionWithLabelOperand(fast, INS_jmp, "while_cond_%d", _whileCondLabelId);
    FlattenedAstAppendLabel(fast, "while_end_%d", _whileEndLabelId);
}
static void InstructionGenIfElse(struct FlattenedAst *fast, struct Ast *node) {
    static unsigned int elseLabelId = 0, ifEndLabelId = 0;
    unsigned int _elseLabelId, _ifEndLabelId;
    InstructionTryGenPush(fast, node->Children[0]);
    /* next if false */
    FlattenedAstGenInstructionWithLabelOperand(fast, INS_jmp_if_false, "else_%d", ++elseLabelId);
    _elseLabelId = elseLabelId;

    /* if { body } */
    InstructionGenBody(fast, node->Children[1]);
    FlattenedAstGenInstructionWithLabelOperand(fast, INS_jmp, "if_end_%d", ++ifEndLabelId);
    _ifEndLabelId = ifEndLabelId;

    /* Alternative */
    FlattenedAstAppendLabel(fast, "else_%d", _elseLabelId);
    if (node->Children[2]) {
        InstructionDispatchGen(fast, node->Children[2]);
    }
    FlattenedAstAppendLabel(fast, "if_end_%d", _ifEndLabelId);
}

static void FlattenedAstResize(struct FlattenedAst *fast) {
    struct FlattenedAstNode **newNodes;
    unsigned int i;
    fast->Capacity *= 2;
    newNodes = calloc(sizeof *newNodes, fast->Capacity);
    for (i = 0; i < fast->Length; ++i) {
        newNodes[i] = fast->Nodes[i];
    }
    free(fast->Nodes);
    fast->Nodes = newNodes;
}

static int FlattenedAstAppend(struct FlattenedAst *fast, struct FlattenedAstNode *node) {
    if (!fast || !node) {
        return R_InvalidArgument;
    }
    if (fast->Length + 1 >= fast->Capacity) {
        FlattenedAstResize(fast);
    }
    fast->Nodes[fast->Length++] = node;
    return R_OK;
}


static void InstructionDispatchGen(struct FlattenedAst *fast,struct Ast *node) {
    if (!node) {
        return ;
    }
    switch(node->Type) {
        case UNASSIGNED:
            return;
            break;
        case Body:
            InstructionGenBody(fast, node);
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
        case BLogicEqExpr:
        case BLogicNotEqExpr:
        case BLogicLtExpr:
        case BLogicLtEqExpr:
        case BLogicGtExpr:
        case BLogicGtEqExpr:
            InstructionGenBinaryExpr(fast, node);
            break;
        case BLogicOrExpr:
            InstructionGenLogicalOrExpr(fast, node);
            break;
        case BLogicAndExpr:
            InstructionGenLogicalAndExpr(fast, node);
            break;
        case AssignExpr:
            InstructionGenAssign(fast, node);
            break;
        case UNegExpr:
        case ULogicNotExpr:
            InstructionGenUnaryExpr(fast, node);
            break;
        case ImportExpr:
            InstructionGenImport(fast, node);
            break;
        case ClassNode:
            InstructionGenClass(fast, node);
            break;
        case FunctionNode:
            InstructionGenFunction(fast, node);
            break;
        case CallExpr:
            InstructionGenCall(fast, node);
            break;
        case ArrayIdxExpr:
            InstructionGenArrayIdx(fast, node);
            break;
        case MemberAccessExpr:
            InstructionGenMemberAccess(fast, node);
            break;
        case ReturnExpr:
            InstructionGenReturn(fast, node);
            break;
        case ContinueExpr:
            InstructionGenContinue(fast);
            break;
        case BreakExpr:
            InstructionGenBreak(fast);
            break;
        case MutExpr:
            InstructionGenMut(fast, node);
            break;
        case ConstExpr:
            InstructionGenConst(fast, node);
            break;
        case ForExpr:
            InstructionGenFor(fast, node);
            break;
        case WhileExpr:
            InstructionGenWhile(fast, node);
            break;
        case IfElseExpr:
            InstructionGenIfElse(fast, node);
            break;
    }
}

static int FlattenAst(struct Ast *ast, struct FlattenedAst **out_flattenedAst) {
    unsigned int i;
    struct Ast *child;
    struct FlattenedAst *list = FlattenedAstAlloc(4);
    if (UNASSIGNED == ast->Type) {
        for (i = 0; i < ast->NumChildren; ++i) {
            child = ast->Children[i];
            InstructionDispatchGen(list, child);
        }
    }
    else {
        InstructionDispatchGen(list, ast);
    }
    *out_flattenedAst = list;
    return R_OK;
}

/**************************** Public Functions ****************************/

int InstructionTypeNumOperands(enum InstructionType ins) {
    if (ins >= INS_NUM_INSTRUCTION_TYPES) {
        return -1;
    }
    return InstructionTypes[ins].NumOperands;
}
const char *InstructionTypeToString(enum InstructionType ins) {
    if (ins >= INS_NUM_INSTRUCTION_TYPES) {
        return NULL;
    }
    return InstructionTypes[ins].AsString;
}

char *InstructionToString(struct Instruction *ins) {
    char *instruction, *operand, *tmp;
    int numOperands, i;
    if (!ins) {
        return NULL;
    }
    instruction = strdup(InstructionTypeToString(ins->Instruction));
    numOperands = InstructionTypeNumOperands(ins->Instruction);
    for (i = 0; i < numOperands; ++i) {
        operand = OperandToString(ins->Operands[0]);
        tmp = str_cat_sep(instruction, ' ', operand);
        free(operand);
        free(instruction);
        instruction = tmp;
    }
    return instruction;
}

char *InstructionToColorizedString(struct Instruction *ins) {
    /* TODO: Colorize! */
    ins = NULL;
    return NULL;
}

int FlattenedAstFlattenAst(struct Ast *ast, struct FlattenedAst **out_flattenedAst) {
    int result;
    struct FlattenedAst *fast;
    if (!ast || !out_flattenedAst) {
        *out_flattenedAst = NULL;
        return R_InvalidArgument;
    }
    symStackInit();
    result = FlattenAst(ast, &fast);
    if (R_OK != result) {
        *out_flattenedAst = NULL;
        return result;
    }
    *out_flattenedAst = fast;
    return R_OK;
}

int InstructionAssemble(struct Instruction *ins, unsigned int *out_assembledIns) {
    if (!ins || !out_assembledIns) {
        return R_InvalidArgument;
    }
    return R_NotYetImplemented;
}
int InstructionDisassemble(unsigned int assembledIns, struct Instruction **out_ins) {
    if (!assembledIns || !out_ins) {
        return R_InvalidArgument;
    }
    return R_NotYetImplemented;
}
