#ifndef _LITTLE_LANG_MODULE_TABLE_H
#define _LITTLE_LANG_MODULE_TABLE_H

#include "symbol_table.h"
#include "type_table.h"
#include "ast.h"

struct Module {
    struct SymbolTable *ModuleScope;
    struct SymbolTable *CurrentScope;
    struct TypeTable *TypeTable;
    struct Ast *Program;
    struct ModuleTable *Imports;
};

struct ModuleTableNode {
    char *Key;
    struct Module *Module;
    struct ModuleTableNode *Next;
};

struct ModuleTable {
    unsigned int NumNodes;
    struct ModuleTableNode **Nodes;
};

int ModuleMake(struct Module *module, struct Ast *program, struct ModuleTable *imports);
int ModuleFree(struct Module *module);

int ModuleTableMake(struct ModuleTable *moduleTable);
int ModuleTableFree(struct ModuleTable *moduleTable);
int ModuleTableInsert(struct ModuleTable *moduleTable, char *key, struct Module *module);
int ModuleTableFind(struct ModuleTable *moduleTable, char *key, struct Module **out_module);

#endif
