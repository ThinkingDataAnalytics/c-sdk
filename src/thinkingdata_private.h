#ifndef THINKINGDATA_ANALYTICS_CORE_H
#define THINKINGDATA_ANALYTICS_CORE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "thinkingdata.h"

typedef int (*ta_consumer_add)(void *this_, const char *event, unsigned long length);
typedef int (*ta_consumer_flush)(void *this_);
typedef int (*ta_consumer_close)(void *this_);

struct TAConsumerOp {
  ta_consumer_add   add;
  ta_consumer_flush flush;
  ta_consumer_close close;
};

struct TAConsumer {
  struct TAConsumerOp op;
  void *this_;
};

enum TANodeType {
    TA_Error   = -1,
    TA_Boolean =  1,
    TA_NUMBER  =  2,
    TA_INT     =  3,
    TA_DATE    =  4,
    TA_STRING  =  5,
    TA_DICT    =  6,
    TA_ARRAY   =  7,
};
typedef int TANodeType;

typedef union TANodeValue {
    int boolean_;
    double number_;
    long long int_;
    struct {
        time_t seconds;
        int microseconds;
    } date_;
    char *string_;
    struct TAListNode *child;
} TANodeValue;

typedef struct TANode {
    char *key;
    
    TANodeType type;
    TANodeValue value;
} TANode;

#define TA_SAFE_MALLOC(n) ta_safe_malloc((n), __LINE__)
void *ta_safe_malloc(unsigned long n, unsigned long line);

#define TA_SAFE_FREE(curr) ta_safe_free((curr), __LINE__)
void ta_safe_free(void *curr, unsigned long line);

typedef struct TAListNode {
    struct TAListNode *next;
    struct TANode *value;
} TAListNode;

#if defined(__linux__)
#define LOCALTIME(seconds, now) localtime_r((seconds), (now))
#define FOPEN(file, filename, option) do { \
*(file) = fopen((filename), (option)); \
} while (0)

#elif defined(__APPLE__)
#define LOCALTIME(seconds, now) localtime_r((seconds), (now))
#define FOPEN(file, filename, option) do { \
*(file) = fopen((filename), (option)); \
} while (0)

#elif defined(_WIN32)
#define LOCALTIME(seconds, now) localtime_s((now), (seconds))
#define FOPEN(file, filename, option) do { \
errno_t err = fopen_s((file), (filename), (option)); \
if (0 != err) { \
*(file) = NULL; \
} \
} while (0)
#define _CRT_SECURE_NO_WARNINGS
#if _MSC_VER
#define snprintf _snprintf
#endif
#endif

#endif  // THINKINGDATA_ANALYTICS_CORE_H