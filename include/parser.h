#ifndef _LITTLE_LANG_PARSER_H
#define _LITTLE_LANG_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token_stream.h"

struct ParsedTrees {
    struct Ast *Imports;
    struct Ast *Classes;
    struct Ast *TopLevelFunctions;
    struct Ast *Program;
};

int Parse(struct ParsedTrees *parsedTrees, struct Lexer *lexer);

int ParseThing(struct Ast **out_ast, struct TokenStream *tokenStream);

#endif
