#include "instruction.h"

#include "helpers/macro_helpers.h"

#include <stdlib.h>


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

char *InstructionToColorizedString(struct Instruction *ins) {
    ins = NULL;
    return NULL;
}
