#ifndef thinkingdata_h
#define thinkingdata_h

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 SDK 错误码
 */
typedef enum {
    TA_OK,
    TA_MALLOC_ERROR,
    TA_INVALID_PARAMETER_ERROR,
    TA_IO_ERROR
} TAErrCode;

/**
 TABool
 */
typedef enum {
    TA_FALSE,
    TA_TRUE
} TABool;

/**
 日志切分模式
 */
typedef enum {
    HOURLY,
    DAILY
} TARotateMode;

/**
 TA_ASSERT
 */
#define TA_ASSERT(condition) do { \
bool cond = (condition); \
if (!cond) { \
printf("condition:%d\n", cond); \
abort(); \
} \
} while (0)

/**
 事件属性
 */
typedef struct TANode TAConfig;

/**
 LoggingConsumer 用于将事件以日志文件的形式记录在本地磁盘中
 */
typedef struct TAConsumer TALoggingConsumer;

/**
 初始化 Logging Consumer
 
 @param  consumer           TALoggingConsumer 实例
 @param  config             配置参数
 
 @return TA_OK:初始化成功
 */
int ta_init_logging_consumer(TALoggingConsumer** consumer, const TAConfig* config);

/**
 ThinkingdataAnalytics 对象
 */
typedef struct ThinkingdataAnalytics ThinkingdataAnalytics;

/**
 初始化 ThinkingdataAnalytics 对象

 @param  consumer           TAConsumer 对象
 @param  ta                 初始化的 ThinkingdataAnalytics 实例

 @return TA_OK              初始化成功
 */
int ta_init(struct TAConsumer* consumer, ThinkingdataAnalytics** ta);

/**
 事件属性
 */
typedef struct TANode TAProperties;
    
/**
 初始化SDK配置信息
 */
TAConfig* ta_init_config(void);

/**
 初始化事件属性或用户属性对象
 
 @return TAProperties 对象
 */
TAProperties* ta_init_properties(void);

/**
 释放事件属性或用户属性对象
 
 @param properties              要释放的 TAProperties 对象
 */
void ta_free_properties(TAProperties* properties);

/**
 自定义事件埋点
 
 @param  account_id             用户 ID
 @param  distinct_id            访客 ID
 @param  event                  事件名称
 @param  properties             事件属性，TAProperties 对象，NULL 表示无事件属性
 @param  ta                     ThinkingdataAnalytics 实例
 @return TA_OK                  埋点成功
 */
int ta_track(const char* account_id,
             const char* distinct_id,
             const char* event,
             const TAProperties* properties,
             ThinkingdataAnalytics* ta);

/**
  设置用户属性
 
  @param  account_id             用户 ID
  @param  distinct_id            访客 ID
  @param  properties             用户属性
  @param  ta                     ThinkingdataAnalytics 对象
  @return TA_OK                  设置成功
 */
int ta_user_set(const char* account_id,
                const char* distinct_id,
                const TAProperties* properties,
                ThinkingdataAnalytics* ta);

/**
  重置用户属性
 
  @param  account_id             用户 ID
  @param  distinct_id            访客 ID
  @param  propertyName           用户属性
  @param  ta                     ThinkingdataAnalytics 对象
  @return TA_OK                  设置成功
 */
int ta_user_unset(const char* account_id,
                  const char* distinct_id,
                  const char* propertyName,
                  ThinkingdataAnalytics* ta);

/**
 设置单次用户属性
 
 @param  account_id              用户 ID
 @param  distinct_id             访客 ID
 @param  properties              用户属性
 @param  ta                      ThinkingdataAnalytics 对象
 @return TA_OK                   设置成功
 */
int ta_user_setOnce(const char* account_id,
                    const char* distinct_id,
                    const TAProperties* properties,
                    ThinkingdataAnalytics* ta);

/**
 对数值类型用户属性进行累加操作

 @param  account_id              用户 ID
 @param  distinct_id             访客 ID
 @param  properties              用户属性，必须为 Number 类型的属性
 @param  ta                      ThinkingdataAnalytics 对象
 @return TA_OK                   设置成功
 */
int ta_user_add(const char* account_id,
                const char* distinct_id,
                const TAProperties* properties,
                ThinkingdataAnalytics* ta);

/**
 删除用户 该操作不可逆 需慎重使用
 
 @param  account_id              用户 ID
 @param  distinct_id             访客 ID
 @param  ta                      ThinkingdataAnalytics 对象
 @return TA_OK                   设置成功
 */
int ta_user_del(const char* account_id,
                const char* distinct_id,
                ThinkingdataAnalytics* ta);

/**
 增添用户属性

 @param  account_id              用户 ID
 @param  distinct_id             访客 ID
 @param  ta                      ThinkingdataAnalytics 对象
 @return TA_OK                   设置成功
*/
int ta_user_append(const char *account_id,
                const char *distinct_id,
                const TAProperties *properties,
                ThinkingdataAnalytics *ta);

/**
 向事件属性或用户属性添加 Bool 类型的属性

 @param  key                    属性名称
 @param  bool_                  TABool 对象，属性值
 @param  properties             TAProperties 对象
 @return TA_OK                  添加成功
 */
int ta_add_bool(const char* key, TABool bool_, TAProperties* properties);

/**
 向事件属性或用户属性添加 Number 类型的属性

 @param  key                    属性名称
 @param  number_                属性值
 @param  properties             TAProperties 对象
 @return TA_OK                  添加成功
 */
int ta_add_number(const char* key, double number_, TAProperties* properties);

/**
 向事件属性或用户属性添加 long integer 类型的属性
 
 @param  key                    属性名称
 @param  int_                   属性值
 @param  properties             TAProperties 对象
 @return TA_OK                  添加成功
 */
int ta_add_int(const char* key, long long int_, TAProperties* properties);

/**
 向事件属性或用户属性添加 Date 类型的属性
 
 @param  key                     属性名称
 @param  seconds                 时间戳，单位为秒
 @param  microseconds            时间戳的毫秒部分
 @param  properties              TAProperties 对象
 @return TA_OK                   添加成功
 */
int ta_add_date(const char* key, time_t seconds, int microseconds, TAProperties* properties);

/**
 向事件属性或用户属性添加 String 类型的属性

 @param  key                     属性名称
 @param  string_                 字符串的句柄
 @param  length                  字符串长度
 @param  properties              TAProperties 对象
 @return TA_OK                   添加成功
 */
int ta_add_string(const char* key, const char* string_, unsigned int length, TAProperties* properties);

/**
向事件属性或用户属性添加 array 类型的属性

@param  key                     属性名称
@param  string_                 字符串的句柄
@param  length                  字符串长度
@param  properties              TAProperties 对象
@return TA_OK                   添加成功
*/
int ta_append_array(const char *key, const char *string_, unsigned int length, TAProperties *properties);

/**
 设置事件公共属性
 */
int ta_set_super_properties(const TAProperties* properties, ThinkingdataAnalytics* ta);

/**
 删除事件某个公共属性
 */
int ta_unset_super_properties(const char* key, ThinkingdataAnalytics* ta);

/**
 删除事件所有公共属性
 */
int ta_clear_super_properties(ThinkingdataAnalytics* ta);


typedef TAProperties* (*ta_dynamic_func)(void);
void ta_set_dynamic_properties(ta_dynamic_func func, ThinkingdataAnalytics *ta);

/**
 释放 ThinkingdataAnalytics 对象

 @param ta              ThinkingdataAnalytics 实例
 */
void ta_free(ThinkingdataAnalytics* ta);

/**
 释放 TALoggingConsumer 对象
 
 @param cosumer         TALoggingConsumer 实例
 */
void ta_consumer_free(TALoggingConsumer* cosumer);
/**
 同步 ThinkingdataAnalytics 的状态，将发送 Consumer 的缓存中所有数据

 @param ta              ThinkingdataAnalytics 实例
 */
void ta_flush(ThinkingdataAnalytics* ta);

#ifdef __cplusplus
}
#endif

#endif /* thinkingdata_h */
