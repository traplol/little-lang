#include "little_lang_machine.h"
#include "helpers/strings.h"

#include <stdio.h>
#include <stdlib.h>

enum LittleLangObjectType {
    LLBuiltinFunctionObject,
    LLFunctionObject,
    LLNilObject,
    LLBooleanObject,
    LLSymbol,
    LLValue,
    LLModule,
};

struct LittleLangObject {
    enum LittleLangObjectType Type;
    union {
        struct {
            struct Symbol *Symbol;
        } Symbol;
        struct {
            struct Value *Value;
        } Value;
        struct {
            struct LittleLangObject *(*Fn)(int argc, struct LittleLangObject **argv);
        } BuiltinFunction;
        struct {
            char *Name;
            struct LittleLangObject **Params;
            unsigned int NumParams;
            struct LittleLangObject **Body;
            unsigned int BodyLen;
        } Function;
        struct {
            unsigned int NumObjects;
            struct LittleLangObject **Objects;
        } Module;
    } u;
};

int LittleLangMachineIsValid(struct LittleLangMachine *llm) {
    return llm && llm->Lexer && llm->GlobalScope && llm->TypeTable;
}

int LittleLangMachineIsInvalid(struct LittleLangMachine *llm) {
    return !LittleLangMachineIsValid(llm);
}

int LittleLangParseUnexecptedTokenError(struct Token *token) {
    fprintf(stderr, "Unexpected token '%s'\n", token->TokenStr);
    return -1;
}

int LittleLangParseExpect(struct LittleLangMachine *llm, enum TokenType type ) {
    struct Token *token;
    if (0 != LexerPeekToken(llm->Lexer, &token)) {
        return 0;
    }
    if (token->Type != type) {
        return 0;
    }
    return 1;
}

int LittleLangMachineDoOpts(struct LittleLangMachine *llm, int argc, char **argv) {
    char *filename = strdup("test.ll");
    char *code = strdup("def HelloWorld {\n"
                        "    print(\"Hello world\")\n"
                        "}");
    if (LittleLangMachineIsInvalid(llm)) {
        return -1;
    }
    llm->CmdOpts.argc = argc;
    llm->CmdOpts.argv = argv;
    llm->CmdOpts.code = code;
    llm->CmdOpts.filename = filename;
    return 0;
}

/************************** Public Functions *****************************/

int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv) {
    int result;

    if (!llm) {
        return -1;
    }
    
    result = LittleLangMachineDoOpts(llm, argc, argv);
    if (0 != result) {
        return result;
    }

    llm->Error = 0;
    
    llm->Lexer = malloc(sizeof(*llm->Lexer));
    result = LexerMake(llm->Lexer, llm->CmdOpts.filename, llm->CmdOpts.code);
    if (0 != result) {
        free(llm->Lexer);
        llm->Lexer = NULL;
        return result;
    }

    llm->GlobalScope = malloc(sizeof(*llm->GlobalScope));
    llm->CurrentScope = llm->GlobalScope;
    result = SymbolTableMakeGlobalScope(llm->GlobalScope);
    if (0 != result) {
        free(llm->GlobalScope);
        llm->GlobalScope = NULL;
        llm->CurrentScope = NULL;
        return result;
    }

    llm->TypeTable = malloc(sizeof(*llm->TypeTable));
    result = TypeTableMake(llm->TypeTable, 0);
    if (0 != result) {
        free(llm->TypeTable);
        llm->TypeTable = NULL;
        return result;
    }
    return 0;
}

int LittleLangMachineRun(struct LittleLangMachine *llm) {
    if (LittleLangMachineIsInvalid(llm)) {
        return -1;
    }
    return -1;
}
