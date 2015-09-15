#include "module_table.h"

#include "result.h"
#include "helpers/strings.h"
#include <stdlib.h>
#include <string.h>

#define MODULE_TABLE_DEFAULT_LENGTH (53U)

int ModuleTableNodeMake(struct ModuleTableNode *moduleTableNode, char *key, struct Module *module) {
    if (!moduleTableNode || !key || !module) {
        return R_InvalidArgument;
    }
    moduleTableNode->Key = strdup(key);
    moduleTableNode->Module = module;
    moduleTableNode->Next = NULL;
    return R_OK;
}
int ModuleTableNodeFree(struct ModuleTableNode *moduleTableNode) {
    struct ModuleTableNode *next;
    if (!moduleTableNode) {
        return R_InvalidArgument;
    }
    
    while (moduleTableNode) {
        next = moduleTableNode->Next;
        ModuleFree(moduleTableNode->Module);
        free(moduleTableNode->Key);
        moduleTableNode->Key = NULL;
        moduleTableNode->Module = NULL;
        moduleTableNode->Next = NULL;
        free(moduleTableNode);
        moduleTableNode = next;
    }
    
    return R_OK;
}

/******************** Public Functions *********************/

int ModuleMake(struct Module *module, struct Ast *program, struct ModuleTable *imports) {
    int result;
    if (!module) {
        return R_InvalidArgument;
    }
    module->ModuleScope = calloc(sizeof *(module->ModuleScope), 1);
    result = SymbolTableMakeGlobalScope(module->ModuleScope);
    if (R_OK != result) {
        free(module->ModuleScope);
        return result;
    }
    module->CurrentScope = module->ModuleScope;

    module->TypeTable = calloc(sizeof *(module->TypeTable), 1);
    result = TypeTableMake(module->TypeTable, 0);
    if (R_OK != result) {
        free(module->TypeTable);
        return result;
    }
    module->Program = program;
    module->Imports = imports;
    return result;
}
int ModuleFree(struct Module *module) {
    if (!module) {
        return R_InvalidArgument;
    }

    SymbolTableFree(module->ModuleScope);
    TypeTableFree(module->TypeTable);
    free(module->ModuleScope);
    free(module->TypeTable);

    module->ModuleScope = NULL;
    module->CurrentScope = NULL;
    module->TypeTable = NULL;
    module->Program = NULL;
    module->Imports = NULL;
    return R_OK;
}

int ModuleTableMake(struct ModuleTable *moduleTable) {
    if (!moduleTable) {
        return R_InvalidArgument;
    }
    moduleTable->NumNodes = MODULE_TABLE_DEFAULT_LENGTH;
    moduleTable->Nodes = calloc(sizeof *(moduleTable->Nodes), MODULE_TABLE_DEFAULT_LENGTH);
    return R_OK;
}
int ModuleTableFree(struct ModuleTable *moduleTable) {
    unsigned int i;
    if (!moduleTable) {
        return R_InvalidArgument;
    }
    for (i = 0; i < moduleTable->NumNodes; ++i) {
        if (moduleTable->Nodes[i]) {
            ModuleTableNodeFree(moduleTable->Nodes[i]);
            moduleTable->Nodes[i] = NULL;
        }
    }
    free(moduleTable->Nodes);
    return R_OK;
}
int ModuleTableInsert(struct ModuleTable *moduleTable, char *key, struct Module *module) {
    unsigned int idx;
    struct ModuleTableNode *node, *prev, *tmp;
    if (!moduleTable || !key || !module) {
        return R_InvalidArgument;
    }
    idx = string_hash(key) % moduleTable->NumNodes;
    if (!moduleTable->Nodes[idx]) { /* Key not in table so we're free to just install it */
        node = malloc(sizeof *node);
        ModuleTableNodeMake(node, key, module);
        moduleTable->Nodes[idx] = node;
        return R_OK;
    }
    /* Check if the module already exists in the chain */
    prev = tmp = moduleTable->Nodes[idx];
    while (tmp) {
        if (0 == strcmp(key, moduleTable->Nodes[idx]->Key)) {
            return R_KeyAlreadyInTable;
        }
        prev = tmp;
        tmp = tmp->Next;
    }
    node = malloc(sizeof *node);
    ModuleTableNodeMake(node, key, module);
    prev->Next = node;
    return R_OK;
}
int ModuleTableFind(struct ModuleTable *moduleTable, char *key, struct Module **out_module) {
    unsigned int idx;
    struct ModuleTableNode *node;
    if (!moduleTable || !key || !out_module) {
        goto module_not_found;
    }
    idx = string_hash(key) % moduleTable->NumNodes;
    if (!moduleTable->Nodes[idx]) {
        goto module_not_found;
    }
    node = moduleTable->Nodes[idx];
    while (node) {
        if (0 == strcmp(key, node->Key)) {
            *out_module = node->Module;
            return 1;
        }
        node = node->Next;
    }
module_not_found:
    if (out_module) {
        *out_module = NULL;
    }
    return 0;
}
