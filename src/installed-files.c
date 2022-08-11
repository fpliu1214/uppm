#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

static int record_installed_files_r(const char * dirPath, size_t offset) {
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
                printf("d %s\n", &filePath[offset]);

                r = record_installed_files_r(filePath, offset);

                if (r != 0) {
                    break;
                }
            } else {
                printf("f %s\n", &filePath[offset]);
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
    int resultCode = record_installed_files_r(installedDirPath, strlen(installedDirPath) + 1);
    return resultCode;
}
