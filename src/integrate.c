#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "core/http.h"
#include "uppm.h"

int uppm_integrate_zsh_completion(const char * outputDir, bool verbose) {
    const char * url = "https://raw.githubusercontent.com/leleliu008/uppm/master/zsh-completion/_uppm";

    char * userHomeDir = getenv("HOME");

    if (userHomeDir == NULL) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    size_t userHomeDirLength = strlen(userHomeDir);

    if (userHomeDirLength == 0) {
        return UPPM_ERROR_ENV_HOME_NOT_SET;
    }

    ////////////////////////////////////////////////////////////////

    struct stat st;

    size_t uppmHomeDirLength = userHomeDirLength + 7;
    char   uppmHomeDir[uppmHomeDirLength];
    snprintf(uppmHomeDir, uppmHomeDirLength, "%s/.uppm", userHomeDir);

    if (stat(uppmHomeDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmHomeDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmHomeDir, S_IRWXU) != 0) {
            perror(uppmHomeDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t zshCompletionDirLength = uppmHomeDirLength + 16;
    char   zshCompletionDir[zshCompletionDirLength];
    snprintf(zshCompletionDir, zshCompletionDirLength, "%s/zsh_completion", uppmHomeDir);

    if (stat(zshCompletionDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", zshCompletionDir);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(zshCompletionDir, S_IRWXU) != 0) {
            perror(zshCompletionDir);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////

    size_t zshCompletionFilePathLength = zshCompletionDirLength + 7;
    char   zshCompletionFilePath[zshCompletionFilePathLength];
    snprintf(zshCompletionFilePath, zshCompletionFilePathLength, "%s/_uppm", zshCompletionDir);

    int ret = http_fetch_to_file(url, zshCompletionFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    if (outputDir == NULL) {
        return UPPM_OK;
    }

    if (stat(outputDir, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", outputDir);
            return UPPM_ERROR;
        }
    } else {
        fprintf(stderr, "'%s\n' directory was expected to be exist, but it was not.\n", outputDir);
        return UPPM_ERROR;
    }

    size_t destFilePathLength = strlen(outputDir) + 7;
    char   destFilePath[destFilePathLength];
    snprintf(destFilePath, destFilePathLength, "%s/_uppm", outputDir);

    if (symlink(zshCompletionFilePath, destFilePath) != 0) {
        perror(destFilePath);
        return UPPM_ERROR;
    } else {
        return UPPM_OK;
    }
}

int uppm_integrate_bash_completion(const char * outputDir, bool verbose) {
    (void)outputDir;
    (void)verbose;
    return UPPM_OK;
}

int uppm_integrate_fish_completion(const char * outputDir, bool verbose) {
    (void)outputDir;
    (void)verbose;
    return UPPM_OK;
}
