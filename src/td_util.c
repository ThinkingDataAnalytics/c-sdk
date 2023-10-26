#include "td_util.h"
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

char *convert_time_to_string(const TANode *time_node) {
    struct tm tm;
    char *buf = (char *) TA_SAFE_MALLOC(64);
    if (buf == NULL) {
        return NULL;
    }
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
    int len;
    char *new_str;

    if (!str)
        return NULL;

    len = (int) strlen(str);
    new_str = (char *) TA_SAFE_MALLOC(len + 1);
    if (new_str == NULL) {
        return NULL;
    }
    memcpy(new_str, str, len);
    new_str[len] = '\0';
    return new_str;
}

static TDBool g_td_enableLog = TD_FALSE;

void td_enableLog(int enable) {
    g_td_enableLog = enable == TD_TRUE ? TD_TRUE : TD_FALSE;
}

void td_logInfo(const char *msg, ...) {
    if (g_td_enableLog == TD_TRUE) {
        va_list ap;

        time_t currentTime;
        struct tm *localTime;
        currentTime = time(NULL);
        localTime = localtime(&currentTime);
        printf("[ThinkingData][%d-%02d-%02d %02d:%02d:%02d] ", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

        va_start(ap, msg);
        vprintf(msg, ap);
        printf("\n");
        va_end(ap);
    }
}

void inner_make_dir(const char *path) {
#ifdef __WIN32__
    mkdir(path);
#else
    mkdir(path, 0777);
#endif
}

void mkdirs(char *path) {
    int i, len;
    char str[1024];
    char tempStr[1024];
    char split_flag;
#ifdef WINVER
    split_flag='\\';
#else
    split_flag = '/';
#endif
    strncpy(str, path, 1024);
    len = (int)strlen(str);
    for (i = 0; i < len; i++) {
        tempStr[i] = str[i];
        if (str[i] == split_flag) {
            if (file_exists(tempStr) != 0) {
                inner_make_dir(tempStr);
            }
        }
    }
    if (file_exists(str) != 0) {
        inner_make_dir(tempStr);
    }
}

int file_exists(const char *filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    return exist == 0 ? 0 : 1;
}
