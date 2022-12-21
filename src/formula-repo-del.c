#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "uppm.h"

int uppm_formula_repo_del(const char * formulaRepoName) {
    if (formulaRepoName == NULL) {
        return UPPM_ARG_IS_NULL;
    }

    if (strcmp(formulaRepoName, "") == 0) {
        return UPPM_ARG_IS_EMPTY;
    }

    if (strcmp(formulaRepoName, "offical-core") == 0) {
        fprintf(stderr, "offical-core is not allowed to delete.\n");
        return UPPM_ERROR;
    }

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ENV_HOME_NOT_SET;
    }

    size_t  formulaRepoDBPathLength = userHomeDirLength + 16;
    char    formulaRepoDBPath[formulaRepoDBPathLength];
    memset (formulaRepoDBPath, 0, formulaRepoDBPathLength);
    sprintf(formulaRepoDBPath, "%s/.uppm/repos.db", userHomeDir);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sqlite3 * db = NULL;

    int resultCode = sqlite3_open(formulaRepoDBPath, &db);

    if (resultCode != SQLITE_OK) {
        fprintf(stderr, "%s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return resultCode;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t deleteSqlLength = 40 + strlen(formulaRepoName);
    char   deleteSql[deleteSqlLength];
    memset(deleteSql, 0, deleteSqlLength);
    sprintf(deleteSql, "DELETE FROM formulaRepo WHERE name='%s';", formulaRepoName);

    char * errorMsg = NULL;

    resultCode = sqlite3_exec(db, deleteSql, NULL, NULL, &errorMsg);

    if (resultCode != SQLITE_OK) {
        fprintf(stderr, "%s\n", errorMsg);
    }

    sqlite3_close(db);

    return resultCode;
}
