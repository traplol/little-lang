#ifndef _LITTLE_LANG_LITTLE_LANG_MACHINE_H
#define _LITTLE_LANG_LITTLE_LANG_MACHINE_H

#include "symbol_table.h"
#include "type_table.h"
#include "lexer.h"
#include "ast.h"

struct LittleLangMachine {
    struct Lexer *Lexer;
    struct SymbolTable *GlobalScope;
    struct SymbolTable *CurrentScope;
    struct TypeTable *TypeTable;
    struct Ast *Program;
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

#endif
