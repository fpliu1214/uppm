#include "uppm.h"

int uppm_formula_repo_update(const char * formulaRepoName) {
    UPPMFormulaRepo * formulaRepo = NULL;

    int ret = uppm_formula_repo_lookup(formulaRepoName, &formulaRepo);

    if (ret == UPPM_OK) {
        ret = uppm_formula_repo_sync(formulaRepo);
    }

    uppm_formula_repo_free(formulaRepo);

    return ret;
}
