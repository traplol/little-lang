#ifndef _LITTLE_LANG_PARSER_H
#define _LITTLE_LANG_PARSER_H

#include "ast.h"
#include "lexer.h"

int Parse(struct Ast **out_functionDef, struct Ast **out_ast, struct Lexer *lexer);

#endif
