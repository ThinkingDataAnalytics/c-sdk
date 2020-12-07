#include "util.h"
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

char *convert_time_to_string(const TANode *time_node) {
    char *buf = (char *) TA_SAFE_MALLOC(64);
    struct tm tm;
    LOCALTIME(&time_node->value.date_.seconds, &tm);
    snprintf(buf, 64, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm.tm_year + 1900,
             tm.tm_mon + 1,
             tm.tm_mday,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec,
             time_node->value.date_.microseconds);
    return buf;
}

char *ta_strdup(const char *str) {
    if (!str)
        return NULL;
    int len = (int) strlen(str);
    char *new_str = (char *) TA_SAFE_MALLOC(len + 1);
    memcpy(new_str, str, len);
    new_str[len] = '\0';
    return new_str;
}

void ta_debug(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    vprintf(msg, ap);
    va_end(ap);
}

void mkdirs(char *path) {
    int i, len;
    char str[1024];
    char split_flag;
#ifdef WINVER
    split_flag='\\';
#else
    split_flag = '/';
#endif
    strncpy(str, path, 1024);
    len = strlen(str);
    char tempStr[1024];
    for (i = 0; i < len; i++) {
        tempStr[i] = str[i];
        if (str[i] == split_flag) {
            if (file_exists(tempStr) != 0) {
                mkdir(tempStr, 0777);
            }
        }
    }
    if (file_exists(str) != 0) {
        mkdir(str, 0777);
    }
    return;
}

int file_exists(const char *filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 0;
    else
        return 1;
}