#include "little_lang_machine.h"
#include "parser.h"
#include "globals.h"
#include "interpreter.h"
#include "helpers/strings.h"
#include "helpers/ast_pretty_printer.h"
#include "value.h"
#include "result.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *SourceDirectory;

int LittleLangMachineIsValid(struct LittleLangMachine *llm) {
    return llm && llm->Lexer && llm->AllImportedModules && llm->ThisModule;
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

char *AbsPath(char *path) {
    char actualPath[PATH_MAX+1];
    if (!FileExists(path)) {
        return NULL;
    }
    if (!realpath(path, actualPath)) {
        return NULL;
    }
    return strdup(actualPath);
}

char *GetPath(char *path) {
    char *tmp;
    if (!FileExists(path)) {
        tmp = str_cat(SourceDirectory, "/");
        path = str_cat(tmp, path);
        free(tmp);
        if (!FileExists(path)) {
            return NULL;
        }
    }
    return AbsPath(path);
}

void ShowHelpMsg(void) {
    fprintf(stderr,
            "usage little-lang [option | file] ... [-args ...]"
            "\n"
            "\n-h --help                     Show this message."
            "\n-P --pretty-print-ast         Pretty print the program's AST."
            "\n-T --time-execution           Times the execution of the program."
            "\n-i                            Enters REPL mode after program execution."
            "\nfile                          The program source to run."
            "\n-args ...                     Passes anything after this flag to the program."
            "\n"
        );
}

#define STR_EQ(s1, s2) (strcmp(s1, s2) == 0)
int LittleLangMachineDoOpts(struct LittleLangMachine *llm, int argc, char **argv) {
    char *filename = NULL, *arg;
    /* Init everything off. */
    memset(&(llm->CmdOpts), 0, sizeof (llm->CmdOpts));
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
        else if (STR_EQ("-i", arg)) {
            llm->CmdOpts.ReplMode = 1;
        }
        else if (!filename && FileExists(arg)) {
            filename = arg;
        }
    }
    llm->CmdOpts.argc = argc;
    llm->CmdOpts.argv = argv;
    if (filename) {
        llm->CmdOpts.code = ReadFile(filename);
        llm->CmdOpts.filename = filename;
        SourceDirectory = strdup(filename);
        dirname(SourceDirectory);
    }
    else {
        llm->CmdOpts.ReplMode = 1;
        llm->CmdOpts.filename = "<stdin>";
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
    if (!functionDefs) {
        return R_OK;
    }
    for (i = 0; i < functionDefs->NumChildren; ++i) {
        result = DefineFunction(symbolTable, functionDefs->Children[i]);
        if (R_OK != result) {
            break;
        }
    }
    return result;
}

int LittleLangMachineMakeLexer(struct LittleLangMachine *llm) {
    int result;
    llm->Lexer = malloc(sizeof(*llm->Lexer));
    result = LexerMake(llm->Lexer, llm->CmdOpts.filename, llm->CmdOpts.code);
    if (R_OK != result) {
        free(llm->Lexer);
        llm->Lexer = NULL;
        return result;
    }
    return R_OK;
}

int LittleLangMachineREPLMode(struct LittleLangMachine *llm) {
    struct TokenStream *tokenStream;
    int result;
    struct Ast *stmt = NULL;
    struct Value *value;
    llm->Lexer->REPL = llm->CmdOpts.ReplMode;
    tokenStream = calloc(sizeof *tokenStream, 1);
    while (1) {
        LexerThrowAwayCode(llm->Lexer);
        TokenStreamFree(tokenStream);
        TokenStreamMake(tokenStream, llm->Lexer);
        AstFree(stmt);

        result = ParseStmt(&stmt, tokenStream);
        if (R_OK != result || !stmt) {
            continue;
        }
        if (llm->CmdOpts.PrettyPrintAst) {
            printf("Ast:\n");
            AstPrettyPrintNode(stmt);
            printf("\n\n");
        }
        if (FunctionNode == stmt->Type) {
            DefineFunction(llm->ThisModule->ModuleScope, stmt);
        }
        else {
            value = InterpreterRunAst(llm->ThisModule, stmt);
            printf(" => %s\n", ValueToString(value));
        }
    }
    return result;
}

int ParseProgramTrees(char *absPath, struct ParsedTrees **out_programTrees) {
    int result;
    struct ParsedTrees *programTrees;
    struct Lexer *lexer;
    char *code;
    if (!absPath || !out_programTrees) {
        return R_InvalidArgument;
    }
    code = ReadFile(absPath);
    lexer = calloc(sizeof *lexer, 1);
    result = LexerMake(lexer, absPath, code);
    if (R_OK != result) {
        return result;
    }
    programTrees = calloc(sizeof *programTrees, 1);
    result = Parse(programTrees, lexer);
    if (R_OK != result) {
        return result;
    }
    *out_programTrees = programTrees;
    return R_OK;
}

int LittleLangMachineLoadModule(struct LittleLangMachine *llm, char *filename, struct Module **out_module);
int ImportModules(struct LittleLangMachine *llm, struct Ast *imports, struct ModuleTable **out_imports) {
    int result;
    struct Ast *moduleName, *as, *import;
    struct ModuleTable *moduleTable;
    struct Module *module;
    char *filename, *absPath;
    unsigned int i;
    if (!llm || !imports || !out_imports) {
        return R_InvalidArgument;
    }
    moduleTable = calloc(sizeof *moduleTable, 1);
    result = ModuleTableMake(moduleTable);
    if (R_OK != result) {
        goto cleanup;
    }
    for (i = 0; i < imports->NumChildren; ++i) {
        import = imports->Children[i];
        moduleName = import->Children[0];
        filename = moduleName->u.Value->v.String->CString;
        absPath = GetPath(filename);
        as = import->Children[1];
        ModuleTableFind(llm->AllImportedModules, absPath, &module);
        if (!module) {
            result = LittleLangMachineLoadModule(llm, filename, &module);   
            ModuleTableInsert(llm->AllImportedModules, absPath, module);
            if (R_OK != result) {
                goto cleanup;
            }
        }
        if (as) {
            ModuleTableInsert(moduleTable, as->u.SymbolName, module);
        }
        else {
            /* TODO: Load into ThisModule */
        }
        free(absPath);
    }

    *out_imports = moduleTable;
    return R_OK;

cleanup:
    ModuleTableFree(moduleTable);
    free(moduleTable);
    return result;
}

int LittleLangMachineLoadModule(struct LittleLangMachine *llm, char *filename, struct Module **out_module) {
    int result;
    struct ParsedTrees *programTrees;
    struct ModuleTable *imports;
    struct Module *module;
    char *absPath;
    if (!llm || !out_module || !filename) {
        return R_InvalidArgument;
    }
    absPath = GetPath(filename);
    if (!absPath) {
        fprintf(stderr, "File not found: %s\n", filename);
        return R_FileNotFound;
    }
    result = ParseProgramTrees(absPath, &programTrees);
    if (R_OK != result) {
        return result;
    }
    result = ImportModules(llm, programTrees->Imports, &imports);
    if (R_OK != result) {
        return result;
    }
    module = calloc(sizeof *module, 1);
    result = ModuleMake(module, programTrees->Program, imports);
    if (R_OK != result) {
        free(module);
        return result;
    }
    DefineTopLevelFunctions(module->ModuleScope, programTrees->TopLevelFunctions);
    InterpreterRunProgram(module);

    *out_module = module;
    return R_OK;
}

int LittleLangMakeModuleLookupTable(struct LittleLangMachine *llm) {
    if (!llm) {
        return R_InvalidArgument;
    }
    llm->AllImportedModules = calloc(sizeof *llm->AllImportedModules, 1);
    return ModuleTableMake(llm->AllImportedModules);
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
    result = LittleLangMachineMakeLexer(llm);
    if (R_OK != result) {
        return result;
    }
    result = LittleLangMakeModuleLookupTable(llm);
    if (R_OK != result) {
        return result;
    }
    return result;
}


int LittleLangMachineRun(struct LittleLangMachine *llm) {
    int result;
    clock_t start, end;
    double time;
    if (LittleLangMachineIsInvalid(llm)) {
        return R_InvalidArgument;
    }
    result = GlobalsInit();
    if (R_OK != result) {
        return result;
    }
    InterpreterInit();
    start = clock();
    LittleLangMachineLoadModule(llm, llm->CmdOpts.filename, &llm->ThisModule);
    end = clock();
    if (llm->CmdOpts.TimeExecution) {
        time = (end - start) * 1.0 / CLOCKS_PER_SEC;
        printf("\nfinished program execution in %fs\n", time);
    }
    if (llm->CmdOpts.ReplMode) {
        LittleLangMachineREPLMode(llm);
    }
    if (llm->CmdOpts.PrettyPrintAst) {
        // printf("Imports:\n");
        // AstPrettyPrint(parsedTrees->Imports);
        // printf("\nFunction definitions:\n");
        // AstPrettyPrint(parsedTrees->TopLevelFunctions);
        printf("\nProgram:\n");
        AstPrettyPrint(llm->ThisModule->Program);
        printf("\n\n");
    }
    return result;
}
