#ifndef _LITTLE_LANG_LITTLE_LANG_MACHINE_H
#define _LITTLE_LANG_LITTLE_LANG_MACHINE_H

struct LittleLangMachine {
    struct Lexer *Lexer;
    struct Parser *Parser;
    struct Table *SymbolTable;
    struct Table *TypeTable;
    struct CmdOptions *CmdOptions;
};
#endif
