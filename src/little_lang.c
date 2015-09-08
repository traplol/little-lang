#include "little_lang_machine.h"
#include "ast.h"

#include <stdlib.h>

int main(int argc, char **argv) {
    int result;
    struct LittleLangMachine llm;
    --argc, ++argv;
    if (0 != LittleLangMachineInit(&llm, argc, argv)) {
        exit(1);
    }
    result = LittleLangMachineRun(&llm);

    AstPrettyPrint(llm.Program);
    return result;
}
