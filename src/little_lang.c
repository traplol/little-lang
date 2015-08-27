#include "little_lang_machine.h"

#include <stdlib.h>

int main(int argc, char **argv) {
    struct LittleLangMachine llm;
    --argc, ++argv;
    if (0 != LittleLangMachineInit(&llm, argc, argv)) {
        exit(1);
    }
    return LittleLangMachineRun(&llm);
}
