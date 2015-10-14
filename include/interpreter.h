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

/* Uses typeinfo to generate the default values for an object */
struct Value *InterpreterBuildObjectWithDefaults(struct Module *module, struct TypeInfo *typeInfo);

struct Value *InterpreterDoCallBuiltinFn(struct Module *module, struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc);
struct Value *InterpreterDoCallFunction(struct Module *module, struct Value *function, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc);

/* Attempts to call a method on an object */
struct Value *InterpreterDispatchMethod(struct Module *module, struct Value *object, char *methodName, unsigned int argc, struct Value **argv, struct SrcLoc srcLoc);

#endif
