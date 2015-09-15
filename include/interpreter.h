#ifndef _LITTLE_LANG_INTERPRETER_H
#define _LITTLE_LANG_INTERPRETER_H

#include "little_lang_machine.h"
#include "ast.h"

/* Initializes the core runtime. */
int InterpreterInit(void);

/* Runs the entire program from top to bottom. */
int InterpreterRunProgram(struct Module *module);

/* Executes a single AST, useful for REPL */
struct Value *InterpreterRunAst(struct Module *module, struct Ast *ast);

#endif
