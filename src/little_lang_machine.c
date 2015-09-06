#include "little_lang_machine.h"
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

int LittleLangMachineDoOpts(struct LittleLangMachine *llm, int argc, char **argv) {
    char *filename = strdup("test.ll");
    char *code = strdup("def HelloWorld {\n"
                        "    print(\"Hello world\")\n"
                        "}\n"
                        "HelloWorld()");
    if (LittleLangMachineIsInvalid(llm)) {
        return R_InvalidArgument;
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
        return R_InvalidArgument;
    }
    
    result = LittleLangMachineDoOpts(llm, argc, argv);
    if (R_OK != result) {
        return result;
    }

    llm->Error = 0;
    
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
    if (LittleLangMachineIsInvalid(llm)) {
        return R_InvalidArgument;
    }
    return R_NotYetImplemented;
}
