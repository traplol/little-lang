#include "little_lang_machine.h"

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

int LittleLangMachineInit(struct LittleLangMachine *llm, int argc, char **argv) {
    llm->CmdOpts.argc = argc;
    llm->CmdOpts.argv = argv;
    return -1;
}
int LittleLangMachineRun(struct LittleLangMachine *llm) {
    llm = 0;
    return -1;
}
