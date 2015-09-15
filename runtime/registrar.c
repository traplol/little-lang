#include "registrar.h"
#include "result.h"

#include "runtime_core.h"

int RegisterRuntimes(void) {
    RegisterRuntime_core();
    return R_OK;
}
