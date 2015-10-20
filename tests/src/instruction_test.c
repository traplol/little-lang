#include "../src/instruction.c"

#include "c_test.h"
#include <limits.h>

TEST(InstructionTypeNumOperandsShouldReturnNegativeOne) {
    assert_eq(-1, InstructionTypeNumOperands(-1), "InstructionTypeNumOperands did not return -1 with -1");
    assert_eq(-1, InstructionTypeNumOperands(INS_NUM_INSTRUCTION_TYPES), "InstructionTypeNumOperands did not return -1 with IS_NUM_INSTRUCTION_TYPES");
    assert_eq(-1, InstructionTypeNumOperands(INT_MAX), "InstructionTypeNumOperands did not return -1 with INT_MAX");
    assert_eq(-1, InstructionTypeNumOperands(INT_MIN), "InstructionTypeNumOperands did not return -1 with INT_MIN");
}

TEST(InstructionTypeNumOperandsShouldReturnNonNegativeValue) {
    assert_gte(0, InstructionTypeNumOperands(INS_binary_add), "InstructionTypeNumOperands did not return non-negative with INS_binary_add");
}

TEST(InstructionTypeNumOperandsShouldReturnCorrectValue) {
    assert_gte(0, InstructionTypeNumOperands(INS_binary_add), "InstructionTypeNumOperands did not return 0 with INS_binary_add");
}


TEST(InstructionTypeToStringShouldReturnNULL) {
    assert_eq(NULL, InstructionTypeToString(-1), "InstructionTypeToString did not return NULL with -1");
    assert_eq(NULL, InstructionTypeToString(INS_NUM_INSTRUCTION_TYPES), "InstructionTypeToString did not return NULL with IS_NUM_INSTRUCTION_TYPES");
    assert_eq(NULL, InstructionTypeToString(INT_MAX), "InstructionTypeToString did not return NULL with INT_MAX");
    assert_eq(NULL, InstructionTypeToString(INT_MIN), "InstructionTypeToString did not return NULL with INT_MIN");
}

TEST(InstructionTypeToStringShouldReturnNonNULLValue) {
    assert_ne(NULL, InstructionTypeToString(INS_binary_add), "InstructionTypeToString did not return non-NULL with INS_binary_add");
}

TEST(InstructionTypeToStringShouldReturnCorrectValue) {
    assert_str_eq("binary_add", InstructionTypeToString(INS_binary_add), "InstructionTypeToString did not return 'binary_add' with INS_binary_add");
}


int main() {
    TEST_RUN(InstructionTypeNumOperandsShouldReturnNegativeOne);
    TEST_RUN(InstructionTypeNumOperandsShouldReturnNonNegativeValue);
    TEST_RUN(InstructionTypeNumOperandsShouldReturnCorrectValue);

    TEST_RUN(InstructionTypeToStringShouldReturnNULL);
    TEST_RUN(InstructionTypeToStringShouldReturnNonNULLValue);
    TEST_RUN(InstructionTypeToStringShouldReturnCorrectValue);

    return 0;
}
