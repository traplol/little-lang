#include "registrar.h"
#include "result.h"

#include "runtime_core.h"

int RegisterRuntimes(struct LittleLangMachine *llm) {
    RegisterRuntime_core(llm);
    return R_OK;
}
