#ifndef thinkingdata_h
#define thinkingdata_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \mainpage
 * # ThinkingData SDK for C
 *
 * <img src="https://user-images.githubusercontent.com/53337625/205621683-ed9b97ef-6a52-4903-a2c0-a955dddebb7d.png" alt="logo" width="50%"/>
 * 
 * This is the [ThinkingData](https://www.thinkingdata.cn)™ SDK for C. Documentation is available on our help center in the following languages:
 *
 * - [English](https://docs.thinkingdata.cn/ta-manual/latest/en/installation/installation_menu/server_sdk/c_sdk_installation/c_sdk_installation.html)
 * - [中文](https://docs.thinkingdata.cn/ta-manual/latest/installation/installation_menu/server_sdk/c_sdk_installation/c_sdk_installation.html)
 * - [日本語](https://docs.thinkingdata.cn/ta-manual/latest/ja/installation/installation_menu/server_sdk/c_sdk_installation/c_sdk_installation.html)
 *
 * ---
 */

/**
 * SDK error code
 */
typedef enum {
    TD_OK,
    TD_MALLOC_ERROR,
    TD_INVALID_PARAMETER_ERROR,
    TD_IO_ERROR
} TDErrCode;

/**
 * TDBool
 */
typedef enum {
    TD_FALSE = 0,
    TD_TRUE = 1
} TDBool;

/**
 * TDDebugMode
 */
typedef enum {
    TD_DEBUG_MODE_NORMAL = 0,
    TD_DEBUG_MODE_DRY_RUN = 1
} TDDebugMode;

/**
 * log file rotate mode
 */
typedef enum {
    TD_HOURLY,
    TD_DAILY
} TDRotateMode;

/**
 * TD_ASSERT
 */
#define TD_ASSERT(condition) do { \
int cond = (int) (condition); \
if (!cond) { \
printf("condition:%d\n", cond); \
abort(); \
} \
} while (0)

/**
 * Enable log
 * @param enable 0: false, 1: true
 */
void td_enableLog(int enable);

/**
 * Consumer's config
 */
typedef struct TANode TDConfig;
/**
 * Custom event properties
 */
typedef struct TANode TDProperties;
/**
 * Data consumer
 */
struct TDConsumer;
/**
 * Entry of SDK
 */
struct TDAnalytics;
/**
 * Init consumer
 * @param consumer consumer ptr
 * @param config  consumer config
 * @return result
 */
int td_init_consumer(struct TDConsumer **consumer, const TDConfig *config);
/**
 * Init SDK
 * @param consumer destination consumer
 * @param ta SDK instance
 * @return result
 */
int td_init(struct TDConsumer *consumer, struct TDAnalytics **ta);
/**
 * Construct config
 * @return TDConfig*
 */
TDConfig* td_init_config(void);
/**
 * Construct properties
 * @return TDProperties*
 */
TDProperties *td_init_properties(void);
/**
 * Construct custom properties
 * @param key key
 * @return TDProperties*
 */
TDProperties *td_init_custom_properties(const char *key);
/**
 * Free memory
 * @param properties ptr
 */
void td_free_properties(TDProperties *properties);
/**
 * Track event
 * @param account_id account id
 * @param distinct_id distinct id
 * @param event event name
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_track(const char *account_id,
             const char *distinct_id,
             const char *event,
             const TDProperties *properties,
             struct TDAnalytics *ta);
/**
 * Updatable event
 * @param account_id account id
 * @param distinct_id distinct id
 * @param event event name
 * @param event_id event id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_track_update(const char *account_id,
                    const char *distinct_id,
                    const char *event,
                    const char *event_id,
                    const TDProperties *properties,
                    struct TDAnalytics *ta);
/**
 * Override event
 * @param account_id account id
 * @param distinct_id distinct id
 * @param event event name
 * @param event_id event id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_track_overwrite(const char *account_id,
                       const char *distinct_id,
                       const char *event,
                       const char *event_id,
                       const TDProperties *properties,
                       struct TDAnalytics *ta);
/**
 * First event
 * @param account_id account id
 * @param distinct_id distinct id
 * @param event event name
 * @param firstCheckId first event flag
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_track_first_event(const char *account_id,
                         const char *distinct_id,
                         const char *event,
                         const char *firstCheckId,
                         TDProperties *properties,
                         struct TDAnalytics *ta);
/**
 * Set user properties
 * @param account_id account id
 * @param distinct_id distinct id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_user_set(const char *account_id,
                const char *distinct_id,
                const TDProperties *properties,
                struct TDAnalytics *ta);
/**
 * Unset user properties
 * @param account_id account id
 * @param distinct_id distinct id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_user_unset(const char *account_id,
                  const char *distinct_id,
                  const char *propertyName,
                  struct TDAnalytics *ta);
/**
 * Set once user properties
 * @param account_id account id
 * @param distinct_id distinct id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_user_setOnce(const char *account_id,
                    const char *distinct_id,
                    const TDProperties *properties,
                    struct TDAnalytics *ta);
/**
 * Add user properties
 * @param account_id account id
 * @param distinct_id distinct id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_user_add(const char *account_id,
                const char *distinct_id,
                const TDProperties *properties,
                struct TDAnalytics *ta);
/**
 * Delete user
 * @param account_id account id
 * @param distinct_id distinct id
 * @param ta SDK ptr
 * @return result
 */
int td_user_delete(const char *account_id,
                const char *distinct_id,
                struct TDAnalytics *ta);
/**
 * Append user properties
 * @param account_id account id
 * @param distinct_id distinct id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_user_append(const char *account_id,
                   const char *distinct_id,
                   const TDProperties *properties,
                   struct TDAnalytics *ta);
/**
 * Uniq append user properties
 * @param account_id account id
 * @param distinct_id distinct id
 * @param properties properties
 * @param ta SDK ptr
 * @return result
 */
int td_user_uniq_append(const char *account_id,
                        const char *distinct_id,
                        const TDProperties *properties,
                        struct TDAnalytics *ta);
/**
 * Add property: bool
 * @param key property key
 * @param bool_ TDBool value
 * @param properties properties
 * @return result
 */
int td_add_bool(const char *key, TDBool bool_, TDProperties *properties);
/**
 * Add property: number
 * @param key property key
 * @param number_ double
 * @param properties properties
 * @return result
 */
int td_add_number(const char *key, double number_, TDProperties *properties);
/**
 * Add property: TDProperties
 * @param subProperties_ TDProperties*
 * @param properties properties
 * @return result
 */
int td_add_property(TDProperties *subProperties_, TDProperties *properties);
/**
 * Add property: int
 * @param key property key
 * @param int_ int
 * @param properties properties
 * @return result
 */
int td_add_int(const char *key, long int_, TDProperties *properties);
/**
 * Add property: date
 * @param key property key
 * @param seconds time_t
 * @param microseconds int
 * @param properties properties
 * @return result
 */
int td_add_date(const char *key, time_t seconds, int microseconds, TDProperties *properties);
/**
 * Add property: string
 * @param key property key
 * @param string_ string
 * @param length length
 * @param properties properties
 * @return result
 */
int td_add_string(const char *key, const char *string_, unsigned int length, TDProperties *properties);
/**
 * Append property: array
 * @param key property key
 * @param string_ string
 * @param length length
 * @param properties properties
 * @return result
 */
int td_append_array(const char *key, const char *string_, unsigned int length, TDProperties *properties);
/**
 * Append property: properties
 * @param key property key
 * @param json TDProperties*
 * @param properties properties
 * @return result
 */
int td_append_properties(const char *key, TDProperties *json, TDProperties *properties);
/**
 * Set super property
 * @param properties TDProperties*
 * @param ta TDAnalytics*
 * @return result
 */
int td_set_super_properties(const TDProperties *properties, struct TDAnalytics *ta);
/**
 * Unset super property
 * @param key property key
 * @param ta TDAnalytics*
 * @return result
 */
int td_unset_super_properties(const char *key, struct TDAnalytics *ta);
/**
 * Clear super properties
 * @param ta TDAnalytics*
 * @return result
 */
int td_clear_super_properties(struct TDAnalytics *ta);
/**
 * Dynamic properties
 */
typedef TDProperties *(*td_dynamic_func)(void);
/**
 * Set dynamic properties
 * @param func td_dynamic_func
 * @param ta TDAnalytics*
 */
void td_set_dynamic_properties(td_dynamic_func func, struct TDAnalytics *ta);
/**
 * Free SDK
 * @param ta TDAnalytics*
 */
void td_free(struct TDAnalytics *ta);
/**
 * Free consumer
 * @param consumer TDConsumer*
 */
void td_consumer_free(struct TDConsumer *consumer);
/**
 * Flush data
 * @param ta TDAnalytics*
 */
void td_flush(struct TDAnalytics *ta);

#ifdef __cplusplus
}
#endif

#endif
