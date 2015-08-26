#ifndef _LITTLE_LANG_LITTLE_LANG_MACHINE_H
#define _LITTLE_LANG_LITTLE_LANG_MACHINE_H

#include "lexer.h"
#include "parser.h"
#include "table.h"

/* TODO: Local stack, scoping, etc. */
struct LittleLangMachine {
    struct Lexer *Lexer;
    struct Parser *Parser;
    struct Table *SymbolTable;
    struct Table *TypeTable;
    struct {
        int argc;
        char **argv;
        char **Filenames;
    } CmdOpts;
};

int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv);
int LittleLangMachineRun(struct LittleLangMachine *llm);

#endif
