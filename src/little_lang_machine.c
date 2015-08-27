#include "little_lang_machine.h"

int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv) {
    llm->CmdOpts.argc = argc;
    llm->CmdOpts.argv = argv;
    return 0;
}
int LittleLangMachineRun(struct LittleLangMachine *llm) {
    llm = 0;
    return 0;
}
