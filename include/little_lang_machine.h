#ifndef _LITTLE_LANG_LITTLE_LANG_MACHINE_H
#define _LITTLE_LANG_LITTLE_LANG_MACHINE_H

#include "symbol_table.h"
#include "type_table.h"
#include "lexer.h"

struct LittleLangMachine {
    struct Lexer *Lexer;
    struct SymbolTable *GlobalScope;
    struct SymbolTable *CurrentScope;
    struct TypeTable *TypeTable;
    struct {
        int argc;
        char **argv;
        char *code;
        char *filename;
    } CmdOpts;
    int Error;
};
int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv);
int LittleLangMachineRun(struct LittleLangMachine *llm);
int LittleLangMachineDenit(struct LittleLangMachine *llm);

#endif
