#ifndef _LITTLE_LANG_LITTLE_LANG_MACHINE_H
#define _LITTLE_LANG_LITTLE_LANG_MACHINE_H

#include "module_table.h"


struct LittleLangMachine {
    struct Lexer *Lexer;
    struct ModuleTable *AllImportedModules;
    struct Module *ThisModule;
    struct {
        int argc;
        char **argv;
        char *code;
        char *filename;
        int PrettyPrintAst;
        int TimeExecution;
        int ReplMode;
    } CmdOpts;
    int Error;
};
int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv);
int LittleLangMachineRun(struct LittleLangMachine *llm);
int LittleLangMachineDenit(struct LittleLangMachine *llm);

#endif
