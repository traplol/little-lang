#ifndef _LITTLE_LANG_INTERPRETER_H
#define _LITTLE_LANG_INTERPRETER_H

#include "little_lang_machine.h"
#include "ast.h"

/* Initializes the core runtime. */
int InterpreterInit(struct LittleLangMachine *llm);

/* Runs the entire program from top to bottom. */
int InterpreterRunProgram(struct LittleLangMachine *llm);

/* Executes a single AST, useful for REPL */
struct Value *InterpreterRunAst(struct LittleLangMachine *llm, struct Ast *ast);

#endif
