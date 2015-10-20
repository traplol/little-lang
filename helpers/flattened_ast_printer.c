#include "flattened_ast_printer.h"

#include <stdio.h>
#include <stdlib.h>

static void PrettyPrintFlattenedAstNode(struct FlattenedAstNode *node) {
    char *tmp;
    if (node->IsInstruction) {
        tmp = InstructionToString(node->u.Instruction);
        printf("%s\n", tmp);
        free(tmp);
    }
    else {
        printf("%s:\n", node->u.LabelName);
    }
}

void PrettyPrintFlattenedAst(struct FlattenedAst *fast) {
    unsigned int i;
    for (i = 0; i < fast->Length; ++i) {
        PrettyPrintFlattenedAstNode(fast->Nodes[i]);
    }
}
