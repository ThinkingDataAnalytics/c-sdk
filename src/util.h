#ifndef util_h
#define util_h

#include <stdio.h>
#include "thinkingdata_private.h"

char *convert_time_to_string(const TANode *time_node);

char *ta_strdup(const char *str);

void ta_debug(const char *msg, ...);

void mkdirs(char *path);

int file_exists(const char *filename);

#endif
