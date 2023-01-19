#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "core/zlib-flate.h"
#include "uppm.h"

int uppm_formula_repo_lookup(const char * formulaRepoName, UPPMFormulaRepo * * formulaRepoPP) {
    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t formulaRepoNameLength = strlen(formulaRepoName);

    size_t formulaRepoDirPathLength = userHomeDirLength + formulaRepoNameLength + 16;
    char   formulaRepoDirPath[formulaRepoDirPathLength];
    memset(formulaRepoDirPath, 0, formulaRepoDirPathLength);
    snprintf(formulaRepoDirPath, formulaRepoDirPathLength, "%s/.uppm/repos.d/%s", userHomeDir, formulaRepoName);

    struct stat st;

    if (stat(formulaRepoDirPath, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", formulaRepoDirPath);
            return UPPM_ERROR;
        }
    } else {
        return UPPM_ERROR_FORMULA_REPO_NOT_FOUND;
    }

    size_t  formulaRepoDatFormatConfigFilePathLength = formulaRepoDirPathLength + 24;
    char    formulaRepoDatFormatConfigFilePath[formulaRepoDatFormatConfigFilePathLength];
    snprintf(formulaRepoDatFormatConfigFilePath, formulaRepoDatFormatConfigFilePathLength, "%s/.uppm-formula-repo.dat", formulaRepoDirPath);

    if (!((stat(formulaRepoDatFormatConfigFilePath, &st) == 0) && S_ISREG(st.st_mode))) {
        return UPPM_ERROR_FORMULA_REPO_NOT_FOUND;
    }

    size_t  formulaRepoYmlFormatConfigFilePathLength = formulaRepoDirPathLength + 24;
    char    formulaRepoYmlFormatConfigFilePath[formulaRepoYmlFormatConfigFilePathLength];
    snprintf(formulaRepoYmlFormatConfigFilePath, formulaRepoYmlFormatConfigFilePathLength, "%s/.uppm-formula-repo.yml", formulaRepoDirPath);

    FILE * inputFile  = NULL;
    FILE * outputFile = NULL;

    inputFile = fopen(formulaRepoDatFormatConfigFilePath, "rb");

    if (inputFile == NULL) {
        perror(formulaRepoDatFormatConfigFilePath);
        return UPPM_ERROR;
    }

    outputFile = fopen(formulaRepoYmlFormatConfigFilePath, "w");

    if (outputFile == NULL) {
        perror(formulaRepoYmlFormatConfigFilePath);
        fclose(inputFile);
        return UPPM_ERROR;
    }

    int ret = zlib_inflate_file_to_file(inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);

    if (ret != 0) {
        return UPPM_ERROR_FORMULA_REPO_IS_BROKEN;
    }

    UPPMFormulaRepo * formulaRepo = NULL;

    ret = uppm_formula_repo_parse(formulaRepoYmlFormatConfigFilePath, &formulaRepo);

    if (ret != UPPM_OK) {
        return ret;
    }

    formulaRepo->name = strdup(formulaRepoName);

    if (formulaRepo->name == NULL) {
        uppm_formula_repo_free(formulaRepo);
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    formulaRepo->path = strdup(formulaRepoDirPath);

    if (formulaRepo->path == NULL) {
        uppm_formula_repo_free(formulaRepo);
        return UPPM_ERROR_MEMORY_ALLOCATE;
    }

    (*formulaRepoPP) = formulaRepo;
    return UPPM_OK;
}
