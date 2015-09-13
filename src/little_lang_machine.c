#include "little_lang_machine.h"
#include "parser.h"
#include "globals.h"
#include "interpreter.h"
#include "helpers/strings.h"
#include "value.h"
#include "result.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int FileExists(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void ShowHelpMsg(void) {
    fprintf(stderr,
            "usage little-lang [option | file] ... [-args ...]"
            "\n"
            "\n-h --help                     Show this message."
            "\n-P --pretty-print-ast         Pretty print the program's AST."
            "\n-T --time-execution           Times the execution of the program."
            "\nfile                          The program source to run."
            "\n-args ...                     Passes anything after this flag to the program."
            "\n"
        );
}

#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)
int LittleLangMachineDoOpts(struct LittleLangMachine *llm, int argc, char **argv) {
    char *filename = NULL, *code, *arg;
    for (; argc; ++argv, --argc) {
        arg = argv[0];
        if(STR_EQ("-h", arg) || STR_EQ("--help", arg)) {
            ShowHelpMsg();
            exit(0);
        }
        else if(STR_EQ("-P", arg) || STR_EQ("--pretty-print-ast", arg)) {
            llm->CmdOpts.PrettyPrintAst = 1;
        }
        else if(STR_EQ("-T", arg) || STR_EQ("--time-execution", arg)) {
            llm->CmdOpts.TimeExecution = 1;
        }
        else if (STR_EQ("-args", arg)) {
            --argc, ++argv;
            break;
        }
        else if (!filename && FileExists(arg)) {
            filename = arg;
        }
    }
    code = ReadFile(filename);
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
    memset(&(llm->CmdOpts), 0, sizeof (llm->CmdOpts));
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

int DefineFunction(struct SymbolTable *symbolTable, struct Ast *function) {
    struct Value *fn = function->u.Value;
    return SymbolTableInsert(symbolTable, fn, fn->v.Function->Name, 1, function->SrcLoc);
}

int DefineTopLevelFunctions(struct SymbolTable *symbolTable, struct Ast *functionDefs) {
    unsigned int i;
    int result;
    for (i = 0; i < functionDefs->NumChildren; ++i) {
        result = DefineFunction(symbolTable, functionDefs->Children[i]);
        if (R_OK != result) {
            break;
        }
    }
    return result;
}

int LittleLangMachineRun(struct LittleLangMachine *llm) {
    struct ParsedTrees *parsedTrees;
    clock_t start, end;
    double time;
    int result;
    if (LittleLangMachineIsInvalid(llm)) {
        return R_InvalidArgument;
    }
    result = GlobalsInit();
    if (R_OK != result) {
        return result;
    }
    parsedTrees = calloc(sizeof *parsedTrees, 1);
    result = Parse(parsedTrees, llm->Lexer);
    if (R_OK == result) {
        llm->Program = parsedTrees->Program;
        if (llm->CmdOpts.PrettyPrintAst) {
            printf("Function definitions:\n");
            AstPrettyPrint(parsedTrees->TopLevelFunctions);
            printf("\nProgram:\n");
            AstPrettyPrint(llm->Program);
            printf("\n\n");
        }
        InterpreterInit(llm);
        DefineTopLevelFunctions(llm->GlobalScope, parsedTrees->TopLevelFunctions);
        start = clock();
        InterpreterRunProgram(llm);
        end = clock();
        if (llm->CmdOpts.TimeExecution) {
            time = (end - start) * 1.0 / CLOCKS_PER_SEC;
            printf("\nfinished program execution in %fs\n", time);
        }
    }
    else {
        llm->Program = NULL;
        AstFree(parsedTrees->Classes);
        AstFree(parsedTrees->TopLevelFunctions);
        AstFree(parsedTrees->Program);
    }
    free(parsedTrees);
    return result;
}
