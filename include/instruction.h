#ifndef _LITTLE_LANG_INSTRUCTION_H
#define _LITTLE_LANG_INSTRUCTION_H

#include "llstring.h"
#include "ast.h"

enum Register {
    REG_FramePointer,
};

enum OperandType {
    OP_Label = 0,
    OP_Register,
    OP_ValueOfRegisterWithOffset,
    OP_nil,
    OP_true,
    OP_false,
    OP_Symbol,
    OP_Integer,
    OP_Real,
    OP_String,

    
    OP_NUM_OPERAND_TYPES
};
struct Operand {
    enum OperandType Type;
    union {
        char *LabelName;
        enum Register Register;
        struct {
            enum Register Register;
            int Offset;
        } ValueOfRegisterWithOffset;
        char *SymbolName;
        int Integer;
        double Real;
        struct LLString *String;
    } u;
};

enum InstructionType {
    INS_binary_add = 0,/* pop b, a -> push a.__add__(b) */
    INS_binary_sub,    /* pop b, a -> push a.__sub__(b) */
    INS_binary_mul,    /* pop b, a -> push a.__mul__(b) */
    INS_binary_div,    /* pop b, a -> push a.__div__(b) */
    INS_binary_mod,    /* pop b, a -> push a.__mod__(b) */
    INS_binary_pow,    /* pop b, a -> push a.__pow__(b) */
    INS_binary_lshift, /* pop b, a -> push a.__lshift__(b) */
    INS_binary_rshift, /* pop b, a -> push a.__rshift__(b) */
    INS_binary_or,     /* pop b, a -> push a.__or__(b) */
    INS_binary_and,    /* pop b, a -> push a.__and__(b) */
    INS_binary_xor,    /* pop b, a -> push a.__xor__(b) */
    INS_binary_cmp_eq, /* pop b, a -> push a.__eq__(b) */
    INS_binary_cmp_neq,/* pop b, a -> push not a.__eq__(b) */
    INS_binary_cmp_lt, /* pop b, a -> push a.__lt__(b) */
    INS_binary_cmp_lte,/* pop b, a -> push a.__lt__(b) OR a.__eq__(b) */
    INS_binary_cmp_gt, /* pop b, a -> push a.__gt__(b) */
    INS_binary_cmp_gte,/* pop b, a -> push a.__gt__(b) OR a.__eq__(b) */

    INS_unary_negative,/* pop a -> push a.__neg__() */
    INS_unary_not,     /* pop a -> push a.__not__() */

    INS_push,          /* push operand to top of stack */
    INS_pop,           /* pop from stack */ // UNUSED?

    INS_jmp_if_false,  /* pop top of stack and jump to operand if it's `false' */
    INS_jmp,           /* jump to operand */
    INS_return,        /* restore $fp and jump to 1($fp) */
    INS_call,          /* pop top of stack and call that function. */

    INS_get_member,    /* Lookup member symbol. */

    INS_assign_to,     /* Peek top of stack and assign to addr operand */
    INS_set_local_mut,     /* Pop top of stack and assign to addr operand, mark mutable */
    INS_set_global_mut,    /* Pop top of stack and assign to global symbol, mark mutable */
    INS_set_local_mut_nil, /* Assign addr operand to nil and mark mutable */
    INS_set_global_mut_nil,/* Assign global symbol to nil and mark mutable */

    INS_set_local_const,   /* Pop top of stack and assign to addr operand, mark immutable */
    INS_set_global_const,  /* Pop top of stack and assign to global symbol, mark immutable */

    INS_NUM_INSTRUCTION_TYPES
};

struct Instruction {
    enum InstructionType Instruction;

    unsigned int NumOperands;
    struct Operand **Operands;
};

struct FlattenedAstNode {
    unsigned int IsInstruction;
    union {
        struct Instruction *Instruction;
        char *LabelName;
    } u;
};

struct FlattenedAst {
    unsigned int Length;
    unsigned int Capacity;
    struct FlattenedAstNode **Nodes;
};

int InstructionTypeNumOperands(enum InstructionType ins);
const char *InstructionTypeToString(enum InstructionType ins);
char *InstructionToString(struct Instruction *ins);

//char *InstructionToColorizedString(struct Instruction *ins);

int FlattenedAstFlattenAst(struct Ast *ast, struct FlattenedAst **flattenedAst);

#endif
