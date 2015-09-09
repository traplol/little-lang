#include "little_lang_machine.h"
#include "parser.h"
#include "globals.h"
#include "helpers/strings.h"
#include "result.h"

#include <stdio.h>
#include <stdlib.h>

int LittleLangMachineIsValid(struct LittleLangMachine *llm) {
    return llm && llm->Lexer && llm->GlobalScope && llm->TypeTable;
}

int LittleLangMachineIsInvalid(struct LittleLangMachine *llm) {
    return !LittleLangMachineIsValid(llm);
}

char *ReadFile(char *filename) {
    unsigned int len;
    int c;
    char *tmp, *code = NULL;
    FILE *file = fopen(filename, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        len = ftell(file);
        rewind(file);
        code = malloc(len+1);
        tmp = code;
        while (EOF != (c = fgetc(file))) {
            *tmp++ = c;
        }
        *tmp = 0;
        fclose(file);
    }
    return code;
}

int LittleLangMachineDoOpts(struct LittleLangMachine *llm, int argc, char **argv) {
    /* TODO: Parse args correctly. */
    char *filename = argv[0];
    char *code = ReadFile(filename);
    llm->CmdOpts.argc = argc;
    llm->CmdOpts.argv = argv;
    llm->CmdOpts.code = code;
    llm->CmdOpts.filename = filename;
    return R_OK;
}

/************************** Public Functions *****************************/

int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv) {
    int result;

    if (!llm) {
        return R_InvalidArgument;
    }
    
    result = LittleLangMachineDoOpts(llm, argc, argv);
    if (R_OK != result) {
        return result;
    }

    llm->Error = R_OK;
    
    llm->Lexer = malloc(sizeof(*llm->Lexer));
    result = LexerMake(llm->Lexer, llm->CmdOpts.filename, llm->CmdOpts.code);
    if (R_OK != result) {
        free(llm->Lexer);
        llm->Lexer = NULL;
        return result;
    }

    llm->GlobalScope = malloc(sizeof(*llm->GlobalScope));
    llm->CurrentScope = llm->GlobalScope;
    result = SymbolTableMakeGlobalScope(llm->GlobalScope);
    if (R_OK != result) {
        free(llm->GlobalScope);
        llm->GlobalScope = NULL;
        llm->CurrentScope = NULL;
        return result;
    }

    llm->TypeTable = malloc(sizeof(*llm->TypeTable));
    result = TypeTableMake(llm->TypeTable, 0);
    if (R_OK != result) {
        free(llm->TypeTable);
        llm->TypeTable = NULL;
        return result;
    }
    return R_OK;
}

int LittleLangMachineRun(struct LittleLangMachine *llm) {
    struct Ast *program;
    int result;
    if (LittleLangMachineIsInvalid(llm)) {
        return R_InvalidArgument;
    }
    result = GlobalsInit();
    if (R_OK != result) {
        return result;
    }

    result = Parse(&program, llm->Lexer);
    if (R_OK == result) {
        llm->Program = program;
    }
    else {
        llm->Program = NULL;
        AstFree(program);
    }
    return result;
}
