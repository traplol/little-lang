#include "little_lang_machine.h"
#include "helpers/strings.h"
#include "result.h"

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
            struct LittleLangObject **Params;
            unsigned int NumParams;
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
            struct LittleLangObject **Objects;
            unsigned int NumObjects;
        } Module;
    } u;
};

struct LittleLangObject *TheNilObject;
struct LittleLangObject *TheTrueObject;
struct LittleLangObject *TheFalseObject;

int LittleLangMachineIsValid(struct LittleLangMachine *llm) {
    return llm && llm->Lexer && llm->GlobalScope && llm->TypeTable;
}

int LittleLangMachineIsInvalid(struct LittleLangMachine *llm) {
    return !LittleLangMachineIsValid(llm);
}

int LittleLangParseUnexecptedTokenError(struct Token *token) {
    fprintf(stderr, "Unexpected token '%s'\n", token->TokenStr);
    return R_UnexpectedToken;
}

int LittleLangParseExpect(struct LittleLangMachine *llm, enum TokenType type ) {
    struct Token *token;
    if (0 != LexerPeekToken(llm->Lexer, &token)) {
        return R_False;
    }
    if (token->Type != type) {
        return R_False;
    }
    return R_True;
}

struct LittleLangObject *LLOMakeValue(struct Value *value) {
    struct LittleLangObject *object = malloc(sizeof *object);
    object->Type = LLValue;
    object->u.Value.Value = value;
    return object;
}
void LittleLangMachineMakeSingletons(void) {
    TheNilObject = malloc(sizeof *TheNilObject);
    TheNilObject->Type = LLNilObject;

    TheTrueObject = malloc(sizeof *TheTrueObject);
    TheTrueObject->Type = LLBooleanObject;

    TheFalseObject = malloc(sizeof *TheFalseObject);
    TheFalseObject->Type = LLBooleanObject;
}

int LLM_isNil(struct LittleLangObject *object) {
    return TheNilObject == object;
}
int LLM_isTrue(struct LittleLangObject *object) {
    return TheTrueObject == object;
}
int LLM_isFalse(struct LittleLangObject *object) {
    return TheFalseObject == object;
}

int LittleLangMachineDoOpts(struct LittleLangMachine *llm, int argc, char **argv) {
    char *filename = strdup("test.ll");
    char *code = strdup("def HelloWorld {\n"
                        "    print(\"Hello world\")\n"
                        "}");
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
    LittleLangMachineMakeSingletons();
    return R_OK;
}

int LittleLangMachineRun(struct LittleLangMachine *llm) {
    if (LittleLangMachineIsInvalid(llm)) {
        return R_InvalidArgument;
    }
    return R_NotYetImplemented;
}
