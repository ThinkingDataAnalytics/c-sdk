#ifndef thinkingdata_h
#define thinkingdata_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SDK error code */
typedef enum {
    TA_OK,
    TA_MALLOC_ERROR,
    TA_INVALID_PARAMETER_ERROR,
    TA_IO_ERROR
} TAErrCode;

/* TABool */
typedef enum {
    TA_FALSE = 0,
    TA_TRUE = 1
} TABool;

/* rotate mode */
typedef enum {
    HOURLY,
    DAILY
} TARotateMode;

/* TA_ASSERT */
#define TA_ASSERT(condition) do { \
int cond = (int)(condition); \
if (!cond) { \
printf("condition:%d\n", cond); \
abort(); \
} \
} while (0)

/* properties */
typedef struct TANode TAConfig;

/* LoggingConsumer */
typedef struct TAConsumer TALoggingConsumer;

int ta_init_logging_consumer(TALoggingConsumer **consumer, const TAConfig *config);

typedef struct ThinkingdataAnalytics ThinkingdataAnalytics;

int ta_init(struct TAConsumer *consumer, ThinkingdataAnalytics **ta);

typedef struct TANode TAProperties;

TAConfig *ta_init_config(void);

TAProperties *ta_init_properties(void);

void ta_free_properties(TAProperties *properties);

int ta_track(const char *account_id,
             const char *distinct_id,
             const char *event,
             const TAProperties *properties,
             ThinkingdataAnalytics *ta);

int ta_track_update(const char *account_id,
                    const char *distinct_id,
                    const char *event,
                    const char *event_id,
                    const TAProperties *properties,
                    ThinkingdataAnalytics *ta);

int ta_track_overwrite(const char *account_id,
                       const char *distinct_id,
                       const char *event,
                       const char *event_id,
                       const TAProperties *properties,
                       ThinkingdataAnalytics *ta);

int ta_track_first_event(const char *account_id,
                         const char *distinct_id,
                         const char *event,
                         const char *firstCheckId,
                         const TAProperties *properties,
                         ThinkingdataAnalytics *ta);

int ta_user_set(const char *account_id,
                const char *distinct_id,
                const TAProperties *properties,
                ThinkingdataAnalytics *ta);

int ta_user_unset(const char *account_id,
                  const char *distinct_id,
                  const char *propertyName,
                  ThinkingdataAnalytics *ta);

int ta_user_setOnce(const char *account_id,
                    const char *distinct_id,
                    const TAProperties *properties,
                    ThinkingdataAnalytics *ta);

int ta_user_add(const char *account_id,
                const char *distinct_id,
                const TAProperties *properties,
                ThinkingdataAnalytics *ta);

int ta_user_del(const char *account_id,
                const char *distinct_id,
                ThinkingdataAnalytics *ta);

int ta_user_append(const char *account_id,
                   const char *distinct_id,
                   const TAProperties *properties,
                   ThinkingdataAnalytics *ta);

int ta_add_bool(const char *key, TABool bool_, TAProperties *properties);

int ta_add_number(const char *key, double number_, TAProperties *properties);

int ta_add_int(const char *key, long long int_, TAProperties *properties);

int ta_add_date(const char *key, time_t seconds, int microseconds, TAProperties *properties);

int ta_add_string(const char *key, const char *string_, unsigned int length, TAProperties *properties);

int ta_append_array(const char *key, const char *string_, unsigned int length, TAProperties *properties);

int ta_set_super_properties(const TAProperties *properties, ThinkingdataAnalytics *ta);

int ta_unset_super_properties(const char *key, ThinkingdataAnalytics *ta);

int ta_clear_super_properties(ThinkingdataAnalytics *ta);

typedef TAProperties *(*ta_dynamic_func)(void);

void ta_set_dynamic_properties(ta_dynamic_func func, ThinkingdataAnalytics *ta);

void ta_free(ThinkingdataAnalytics *ta);

void ta_consumer_free(TALoggingConsumer *cosumer);

void ta_flush(ThinkingdataAnalytics *ta);

#ifdef __cplusplus
}
#endif

#endif
