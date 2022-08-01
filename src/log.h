#ifndef UPPM_LOG_H
#define UPPM_LOG_H

#include <stdio.h>

#define COLOR_RED    "\033[0;31m"
#define COLOR_GREEN  "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE   "\033[0;94m"
#define COLOR_PURPLE "\033[0;35m"
#define COLOR_OFF    "\033[0m"

#define LOG_INFO(x)    fprintf(stderr, "%s🔔  %s%s\n", COLOR_YELLOW, x, COLOR_OFF)
#define LOG_WARN(x)    fprintf(stderr, "%s🔥  %s%s\n", COLOR_YELLOW, x, COLOR_OFF)
#define LOG_SUCCESS(x) fprintf(stderr, "%s[✔] %s%s\n", COLOR_GREEN,  x, COLOR_OFF)
#define LOG_ERROR(x)   fprintf(stderr, "%s💔  %s%s\n", COLOR_RED,    x, COLOR_OFF)

#define LOG_WHITE(x) fprintf(stderr, "%s\n", x)
#define LOG_GREEN(x) fprintf(stderr, "%s%s%s\n", COLOR_GREEN,  x, COLOR_OFF)
#define LOG_RED(x)   fprintf(stderr, "%s%s%s\n", COLOR_RED,    x, COLOR_OFF)

#endif
