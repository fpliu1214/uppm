#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "core/fs.h"
#include "uppm.h"

static int record_installed_files_r(const char * dirPath, size_t offset, FILE * installedFilesConfigFile) {
    if ((dirPath == NULL) || (strcmp(dirPath, "") == 0)) {
        return 1;
    }

    DIR * dir;
    struct dirent * dir_entry;

    dir = opendir(dirPath);

    if (dir == NULL) {
        perror(dirPath);
        return 2;
    }

    int r = 0;

    while ((dir_entry = readdir(dir))) {
        if ((strcmp(dir_entry->d_name, ".") == 0) || (strcmp(dir_entry->d_name, "..") == 0)) {
            continue;
        }

        size_t filePathLength = strlen(dirPath) + strlen(dir_entry->d_name) + 2;
        char   filePath[filePathLength];
        memset(filePath, 0, filePathLength);
        sprintf(filePath, "%s/%s", dirPath, dir_entry->d_name);

        struct stat st;

        r = stat(filePath, &st);

        if (r == 0) {
            if (S_ISDIR(st.st_mode)) {
                fprintf(installedFilesConfigFile, "d %s/\n", &filePath[offset]);

                r = record_installed_files_r(filePath, offset, installedFilesConfigFile);

                if (r != 0) {
                    break;
                }
            } else {
                fprintf(installedFilesConfigFile, "f %s\n", &filePath[offset]);
            }
        } else {
            perror(filePath);
            break;
        }
    }

    closedir(dir);

    return r;
}

int record_installed_files(const char * installedDirPath) {
    size_t installedDirLength = strlen(installedDirPath);

    size_t  installedFilesConfigFilePathLength = installedDirLength + 25;
    char    installedFilesConfigFilePath[installedFilesConfigFilePathLength];
    memset (installedFilesConfigFilePath, 0, installedFilesConfigFilePathLength);
    sprintf(installedFilesConfigFilePath, "%s/installed-files", installedDirPath);

    if (exists_and_is_a_regular_file(installedFilesConfigFilePath)) {
        return UPPM_OK;
    }

    FILE * installedFilesConfigFile = fopen(installedFilesConfigFilePath, "w");

    if (installedFilesConfigFile == NULL) {
        perror(installedFilesConfigFilePath);
        return UPPM_INSTALLED_FILES_CONFIG_FILE_OPEN_ERROR;
    }

    int resultCode = record_installed_files_r(installedDirPath, installedDirLength + 1, installedFilesConfigFile);

    fclose(installedFilesConfigFile);

    return resultCode;
}