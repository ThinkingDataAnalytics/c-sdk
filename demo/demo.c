#include <string.h>

#include <thinkingdata.h>

TAProperties *dynamic_properties_func(void);

struct TAConsumer* getDebugConsumer(void);

struct TAConsumer* getLoggingConsumer(void);

struct TAConsumer* getBatchConsumer(void);

struct TAConsumer* getAsyncBatchConsumer(void);

int main(int args, char **argv) {
    ThinkingdataAnalytics* ta = NULL;
    struct TAConsumer* consumer = NULL;
    const char* distinct_id = "ABC123";
    const char* account_id = "TA_10001";
    TAProperties* super_properties = ta_init_properties();
    TAProperties* properties = ta_init_properties();
    TAProperties* child_1 = ta_init_custom_properties("child_1");
    TAProperties* child_2 = ta_init_custom_properties("child_2");
    TAProperties* user_properties = ta_init_properties();
    TAProperties* array_properties = ta_init_properties();

    (void)(args);
    (void)(argv);

    /*consumer = getDebugConsumer();*/

    consumer = getLoggingConsumer();

    /*consumer = getBatchConsumer();*/

    /*consumer = getAsyncBatchConsumer();*/

    if (consumer == NULL) return 0;

    if (TA_OK != ta_init(consumer, &ta)) {
        fprintf(stderr, "Failed to initialize the SDK.");
        return 1;
    }

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event_norma", NULL, ta));

    TA_ASSERT(TA_OK == ta_add_string("super_property_key", "super_property_value", strlen("super_property_value"),
        super_properties));
    TA_ASSERT(TA_OK == ta_add_string("super_property_key2", "super_property_value2", strlen("super_property_value2"),
        super_properties));

    ta_set_super_properties(super_properties, ta);
    ta_free_properties(super_properties);

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event_super_property_1", NULL, ta));

    ta_unset_super_properties("super_property_key2", ta);

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event_super_property_2", NULL, ta));

    ta_clear_super_properties(ta);

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event_normal", NULL, ta));

    ta_set_dynamic_properties(&dynamic_properties_func, ta);

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event_dynamic_super_property", NULL, ta));

    ta_set_dynamic_properties(NULL, ta);

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event_normal", NULL, ta));

    ta_track_first_event(account_id, distinct_id, "first", "first_id_1", NULL, ta);

    ta_track_update(account_id, distinct_id, "event_update", "event_id_1", NULL, ta);

    ta_track_overwrite(account_id, distinct_id, "event_overwrite", "event_id_1", NULL, ta);

    TA_ASSERT(TA_OK == ta_add_string("a2", "b2", strlen("b2"), child_1));
    TA_ASSERT(TA_OK == ta_add_string("product_name", "card", strlen("card"), child_1));
    TA_ASSERT(TA_OK == ta_add_number("price", 30.989, child_1));
    TA_ASSERT(TA_OK == ta_add_property(child_1, properties));

    TA_ASSERT(TA_OK == ta_add_string("a2", "b2", strlen("b2"), child_2));
    TA_ASSERT(TA_OK == ta_add_string("product_name", "card", strlen("card"), child_2));
    TA_ASSERT(TA_OK == ta_add_number("price", 30.989, child_2));
    TA_ASSERT(TA_OK == ta_append_properties("arrayList", child_2, properties));

    TA_ASSERT(TA_OK == ta_add_date("#time", time(NULL), 0, properties));
    TA_ASSERT(TA_OK == ta_add_string("#ip", "192.168.1.1", strlen("192.168.1.1"), properties));
    TA_ASSERT(TA_OK == ta_add_string("#app_id", "test_1", strlen("test_1"), properties));
    TA_ASSERT(TA_OK == ta_add_string("product_name", "card", strlen("card"), properties));
    TA_ASSERT(TA_OK == ta_add_number("price", 30.989, properties));
    TA_ASSERT(TA_OK == ta_add_int("coin", -30, properties));
    TA_ASSERT(TA_OK == ta_add_string("order_id", "abc_123", strlen("abc_123"), properties));
    TA_ASSERT(TA_OK == ta_add_date("login_time", time(NULL), 0, properties));
    TA_ASSERT(TA_OK == ta_add_bool("is_firstBuy", TA_TRUE, properties));

    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name1", strlen("product_name1"), properties));
    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name2", strlen("product_name2"), properties));

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "event", properties, ta));

    ta_free_properties(properties);

    TA_ASSERT(TA_OK == ta_add_int("Level", 3, user_properties));
    TA_ASSERT(TA_OK == ta_add_int("test_unset", 1, user_properties));
    TA_ASSERT(TA_OK == ta_user_set(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_add(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_add_int("Age", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_setOnce(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_unset(account_id, distinct_id, "test_unset", ta));
    TA_ASSERT(TA_OK == ta_user_del(account_id, distinct_id, ta));

    ta_free_properties(user_properties);

    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name3", strlen("product_name3"), array_properties));
    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name4", strlen("product_name4"), array_properties));
    TA_ASSERT(TA_OK == ta_user_append(account_id, distinct_id, array_properties, ta));

    TA_ASSERT(TA_OK == ta_user_uniq_append(account_id, distinct_id, array_properties, ta));

    ta_free_properties(array_properties);

    ta_flush(ta);
    ta_free(ta);
    ta_consumer_free(consumer);

    return 0;
}

TAProperties *dynamic_properties_func(void) {
    TAProperties *properties = ta_init_properties();
    TAProperties *json_super_dyld = ta_init_custom_properties("json_super_dyld");
    TA_ASSERT(TA_OK == ta_add_date("dynamic", time(NULL), 0, properties));
    TA_ASSERT(TA_OK == ta_add_string("a3", "b3", strlen("b3"), json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_string("product_name", "card", strlen("card"), json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_number("price", 30.989, json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_int("coin", -30, json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_string("order_id", "abc_123", strlen("abc_123"), json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_date("login_time", time(NULL), 0, json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_bool("is_firstBuy", TA_TRUE, json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_bool("is_test", TA_FALSE, json_super_dyld));
    TA_ASSERT(TA_OK == ta_add_property(json_super_dyld, properties));
    return properties;
}

struct TAConsumer* getDebugConsumer(void) {
    char* serverUrl = "https://receiver-ta-demo.thinkingdata.cn";
    char* appId = "cf8bb16389af47bd9752b503142a7de9";

    TAConfig* config = ta_init_config();
    struct TAConsumer* consumer = NULL;

    TA_ASSERT(TA_OK == ta_add_string("device_id", "123456789", strlen("123456789"), config));
    TA_ASSERT(TA_OK == ta_add_int("timeout", 60, config));
    /* debug_mode, 0 report to TE, 1 is not. */
    TA_ASSERT(TA_OK == ta_add_int("debug_mode", 1, config));
    TA_ASSERT(TA_OK == ta_add_int("log", 1, config));
    TA_ASSERT(TA_OK == ta_add_string("push_url", serverUrl, strlen(serverUrl), config));
    TA_ASSERT(TA_OK == ta_add_string("appid", appId, strlen(appId), config));

    if (TA_OK != ta_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    ta_free_properties(config);
    return consumer;
}

struct TAConsumer* getLoggingConsumer(void) {
    struct TAConsumer* consumer = NULL;
    TAConfig* config = ta_init_config();
    char* logPath = "H:/c_sdk_dev_log";
    TA_ASSERT(TA_OK == ta_add_string("file_path", logPath, strlen(logPath), config));
    TA_ASSERT(TA_OK == ta_add_int("rotate_mode", DAILY, config));
    TA_ASSERT(TA_OK == ta_add_int("file_size", 1024, config));
    TA_ASSERT(TA_OK == ta_add_bool("log", TA_TRUE, config));

    if (TA_OK != ta_init_logging_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    ta_free_properties(config);
    return consumer;
}

struct TAConsumer* getBatchConsumer(void) {
    char* serverUrl = "https://receiver-ta-demo.thinkingdata.cn";
    char* appId = "cf8bb16389af47bd9752b503142a7de9";

    TAConfig* config = ta_init_config();
    struct TAConsumer* consumer = NULL;

    TA_ASSERT(TA_OK == ta_add_string("push_url", serverUrl, strlen(serverUrl), config));
    TA_ASSERT(TA_OK == ta_add_string("appid", appId, strlen(appId), config));
    TA_ASSERT(TA_OK == ta_add_int("batch_size", 10, config));

    if (TA_OK != ta_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    ta_free_properties(config);
    return consumer;
}

struct TAConsumer* getAsyncBatchConsumer(void) {
    char* serverUrl = "https://receiver-ta-demo.thinkingdata.cn";
    char* appId = "cf8bb16389af47bd9752b503142a7de9";

    TAConfig* config = ta_init_config();
    struct TAConsumer* consumer = NULL;

    TA_ASSERT(TA_OK == ta_add_int("log", 1, config));
    TA_ASSERT(TA_OK == ta_add_string("push_url", serverUrl, strlen(serverUrl), config));
    TA_ASSERT(TA_OK == ta_add_string("appid", appId, strlen(appId), config));
    TA_ASSERT(TA_OK == ta_add_int("batch_size", 10, config));

    if (TA_OK != ta_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    ta_free_properties(config);
    return consumer;
}
