#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>

#include "uppm.h"

int uppm_search(const char * keyword) {
    if (keyword == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    size_t keywordLength = strlen(keyword);

    if (keywordLength == 0) {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    UPPMFormulaRepoList * formulaRepoList = NULL;

    int ret = uppm_formula_repo_list(&formulaRepoList);

    if (ret != UPPM_OK) {
        return ret;
    }

    bool isFirst = true;

    for (size_t i = 0; i < formulaRepoList->size; i++) {
        char * formulaRepoPath  = formulaRepoList->repos[i]->path;

        size_t formulaDirLength = strlen(formulaRepoPath) + 10;
        char   formulaDir[formulaDirLength];
        snprintf(formulaDir, formulaDirLength, "%s/formula", formulaRepoPath);

        struct stat status;

        if (stat(formulaDir, &status) != 0) {
            continue;
        }

        if (!S_ISDIR(status.st_mode)) {
            continue;
        }

        DIR           * dir;
        struct dirent * dir_entry;

        dir = opendir(formulaDir);

        if (dir == NULL) {
            perror(formulaDir);
            uppm_formula_repo_list_free(formulaRepoList);
            return UPPM_ERROR;
        }

        while ((dir_entry = readdir(dir))) {
            //puts(dir_entry->d_name);
            if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
                continue;
            }

            size_t patternLength = keywordLength + 7;
            char   pattern[patternLength];
            snprintf(pattern, patternLength, "*%s*.yml", keyword);

            int r = fnmatch(pattern, dir_entry->d_name, 0);

            if (r == 0) {
                size_t  fileNameLength = strlen(dir_entry->d_name);

                dir_entry->d_name[fileNameLength - 4] = '\0';

                if (isFirst) {
                    isFirst = false;
                } else {
                    printf("\n");
                }

                ret = uppm_info(dir_entry->d_name, NULL);

                if (ret != UPPM_OK) {
                    uppm_formula_repo_list_free(formulaRepoList);
                    closedir(dir);
                    return ret;
                }
            } else if(r == FNM_NOMATCH) {
                ;
            } else {
                uppm_formula_repo_list_free(formulaRepoList);
                fprintf(stderr, "fnmatch() error\n");
                closedir(dir);
                return UPPM_ERROR;
            }
        }

        closedir(dir);
    }

    uppm_formula_repo_list_free(formulaRepoList);

    return UPPM_OK;
}
