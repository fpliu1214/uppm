#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "core/rm-r.h"

#include "uppm.h"

static int mkdir_p(const char * dir, bool verbose) {
    if (dir == NULL) {
        return UPPM_ERROR_ARG_IS_NULL;
    }

    if (dir[0] == '\0') {
        return UPPM_ERROR_ARG_IS_EMPTY;
    }

    size_t len = strlen(dir);

    char buf[len + 1U];

    memset(buf, 0, len + 1U);

    struct stat st;

    int i = 0;

    for (;;) {
        char c = dir[i];

        if (c == '/' || c == '\0') {
            if (buf[0] != '\0') {
                if (stat(buf, &st) == 0) {
                    if (!S_ISDIR(st.st_mode)) {
                        fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", buf);
                        return UPPM_ERROR;
                    }
                } else {
                    if (verbose) printf("mkdir -p %s\n", buf);

                    if (mkdir(buf, S_IRWXU) != 0) {
                        if (errno == EEXIST) {
                            if (stat(buf, &st) == 0) {
                                if (!S_ISDIR(st.st_mode)) {
                                    fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", buf);
                                    return UPPM_ERROR;
                                }
                            }
                        } else {
                            perror(buf);
                            return UPPM_ERROR;
                        }
                    }
                }
            }
        }

        if (c == '\0') {
            return 0;
        } else {
            buf[i] = c;
            i++;
        }
    }
}

int uppm_integrate_zsh_completion(const char * outputDIR, bool verbose) {
    char   uppmHomeDIR[256] = {0};
    size_t uppmHomeDIRLength;

    int ret = uppm_home_dir(uppmHomeDIR, 255, &uppmHomeDIRLength);

    if (ret != UPPM_OK) {
        return ret;
    }

    ///////////////////////////////////////////////////////////////////////////////////////

    struct stat st;

    size_t   uppmRunDIRLength = uppmHomeDIRLength + 5U;
    char     uppmRunDIR[uppmRunDIRLength];
    snprintf(uppmRunDIR, uppmRunDIRLength, "%s/run", uppmHomeDIR);

    if (stat(uppmRunDIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", uppmRunDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(uppmRunDIR, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                perror(uppmRunDIR);
                return UPPM_ERROR;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////

    size_t   sessionDIRLength = uppmRunDIRLength + 20U;
    char     sessionDIR[sessionDIRLength];
    snprintf(sessionDIR, sessionDIRLength, "%s/%d", uppmRunDIR, getpid());

    if (stat(sessionDIR, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (rm_r(sessionDIR, false) != 0) {
                perror(sessionDIR);
                return UPPM_ERROR;
            }
        } else {
            fprintf(stderr, "'%s\n' was expected to be a directory, but it was not.\n", sessionDIR);
            return UPPM_ERROR;
        }
    } else {
        if (mkdir(sessionDIR, S_IRWXU) != 0) {
            perror(sessionDIR);
            return UPPM_ERROR;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   tmpFilePathLength = sessionDIRLength + 7U;
    char     tmpFilePath[tmpFilePathLength];
    snprintf(tmpFilePath, tmpFilePathLength, "%s/_uppm", sessionDIR);

    const char * const url = "https://raw.githubusercontent.com/leleliu008/uppm/master/uppm-zsh-completion";

    ret = uppm_http_fetch_to_file(url, tmpFilePath, verbose, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   defaultOutputDIRLength = uppmHomeDIRLength + 26U;
    char     defaultOutputDIR[defaultOutputDIRLength];
    snprintf(defaultOutputDIR, defaultOutputDIRLength, "%s/share/zsh/site-functions", uppmHomeDIR);

    size_t outputDIRLength;

    if (outputDIR == NULL) {
        outputDIR       = defaultOutputDIR;
        outputDIRLength = defaultOutputDIRLength;
    } else {
        outputDIRLength = strlen(outputDIR);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    ret = mkdir_p(outputDIR, verbose);

    if (ret != UPPM_OK) {
        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    size_t   outputFilePathLength = outputDIRLength + 7U;
    char     outputFilePath[outputFilePathLength];
    snprintf(outputFilePath, outputFilePathLength, "%s/_uppm", outputDIR);

    if (rename(tmpFilePath, outputFilePath) != 0) {
        if (errno == EXDEV) {
            ret = uppm_copy_file(tmpFilePath, outputFilePath);

            if (ret != UPPM_OK) {
                return ret;
            }
        } else {
            perror(outputFilePath);
            return UPPM_ERROR;
        }
    }

    printf("zsh completion script for uppm has been written to %s\n", outputFilePath);
    return UPPM_OK;
}

int uppm_integrate_bash_completion(const char * outputDIR, bool verbose) {
    (void)outputDIR;
    (void)verbose;
    return UPPM_OK;
}

int uppm_integrate_fish_completion(const char * outputDIR, bool verbose) {
    (void)outputDIR;
    (void)verbose;
    return UPPM_OK;
}
