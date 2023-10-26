#include <string.h>

#include <thinkingdata.h>

TDProperties *dynamic_properties_func(void);

struct TDConsumer* getDebugConsumer(void);

struct TDConsumer* getLoggingConsumer(void);

struct TDConsumer* getBatchConsumer(void);

struct TDConsumer* getAsyncBatchConsumer(void);

int main(int args, char **argv) {
    struct TDAnalytics* ta = NULL;
    struct TDConsumer* consumer = NULL;
    const char* distinct_id = "ABC123";
    const char* account_id = "TD_10001";

    TDProperties* super_properties = td_init_properties();
    TDProperties* properties = td_init_properties();
    TDProperties* child_1 = td_init_custom_properties("child_1");
    TDProperties* child_2 = td_init_custom_properties("child_2");
    TDProperties* user_properties = td_init_properties();
    TDProperties* array_properties = td_init_properties();

    td_enableLog(1);

    (void)(args);
    (void)(argv);

    /**
     * e.g.
     *
     * consumer = getLoggingConsumer();
     * consumer = getDebugConsumer();
     * consumer = getBatchConsumer();
     * consumer = getAsyncBatchConsumer();
     */
    consumer = getLoggingConsumer();

    if (consumer == NULL) return 0;

    if (TD_OK != td_init(consumer, &ta)) {
        fprintf(stderr, "Failed to initialize the SDK.");
        return 1;
    }

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event_norma", NULL, ta));

    TD_ASSERT(TD_OK == td_add_string("super_property_key", "super_property_value", strlen("super_property_value"),
        super_properties));
    TD_ASSERT(TD_OK == td_add_string("super_property_key2", "super_property_value2", strlen("super_property_value2"),
        super_properties));

    td_set_super_properties(super_properties, ta);
    td_free_properties(super_properties);

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event_super_property_1", NULL, ta));

    td_unset_super_properties("super_property_key2", ta);

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event_super_property_2", NULL, ta));

    td_clear_super_properties(ta);

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event_normal", NULL, ta));

    td_set_dynamic_properties(&dynamic_properties_func, ta);

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event_dynamic_super_property", NULL, ta));

    td_set_dynamic_properties(NULL, ta);

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event_normal", NULL, ta));

    td_track_first_event(account_id, distinct_id, "first", "first_id_1", NULL, ta);

    td_track_update(account_id, distinct_id, "event_update", "event_id_1", NULL, ta);

    td_track_overwrite(account_id, distinct_id, "event_overwrite", "event_id_1", NULL, ta);

    TD_ASSERT(TD_OK == td_add_string("a2", "b2", strlen("b2"), child_1));
    TD_ASSERT(TD_OK == td_add_string("product_name", "card", strlen("card"), child_1));
    TD_ASSERT(TD_OK == td_add_number("price", 30.989, child_1));
    TD_ASSERT(TD_OK == td_add_property(child_1, properties));

    TD_ASSERT(TD_OK == td_add_string("a2", "b2", strlen("b2"), child_2));
    TD_ASSERT(TD_OK == td_add_string("product_name", "card", strlen("card"), child_2));
    TD_ASSERT(TD_OK == td_add_number("price", 30.989, child_2));
    TD_ASSERT(TD_OK == td_append_properties("arrayList", child_2, properties));

    TD_ASSERT(TD_OK == td_add_date("#time", time(NULL), 0, properties));
    TD_ASSERT(TD_OK == td_add_string("#ip", "192.168.1.1", strlen("192.168.1.1"), properties));
    TD_ASSERT(TD_OK == td_add_string("#app_id", "test_1", strlen("test_1"), properties));
    TD_ASSERT(TD_OK == td_add_string("product_name", "card", strlen("card"), properties));
    TD_ASSERT(TD_OK == td_add_number("price", 30.989, properties));
    TD_ASSERT(TD_OK == td_add_int("coin", -30, properties));
    TD_ASSERT(TD_OK == td_add_string("order_id", "abc_123", strlen("abc_123"), properties));
    TD_ASSERT(TD_OK == td_add_date("login_time", time(NULL), 0, properties));
    TD_ASSERT(TD_OK == td_add_bool("is_firstBuy", TD_TRUE, properties));

    TD_ASSERT(TD_OK == td_append_array("product_buy", "product_name1", strlen("product_name1"), properties));
    TD_ASSERT(TD_OK == td_append_array("product_buy", "product_name2", strlen("product_name2"), properties));

    TD_ASSERT(TD_OK == td_track(account_id, distinct_id, "event", properties, ta));

    td_free_properties(properties);

    TD_ASSERT(TD_OK == td_add_int("Level", 3, user_properties));
    TD_ASSERT(TD_OK == td_add_int("test_unset", 1, user_properties));
    TD_ASSERT(TD_OK == td_user_set(account_id, distinct_id, user_properties, ta));
    TD_ASSERT(TD_OK == td_user_add(account_id, distinct_id, user_properties, ta));
    TD_ASSERT(TD_OK == td_add_int("Age", 3, user_properties));
    TD_ASSERT(TD_OK == td_user_setOnce(account_id, distinct_id, user_properties, ta));
    TD_ASSERT(TD_OK == td_user_unset(account_id, distinct_id, "test_unset", ta));
    TD_ASSERT(TD_OK == td_user_delete(account_id, distinct_id, ta));

    td_free_properties(user_properties);

    TD_ASSERT(TD_OK == td_append_array("product_buy", "product_name3", strlen("product_name3"), array_properties));
    TD_ASSERT(TD_OK == td_append_array("product_buy", "product_name4", strlen("product_name4"), array_properties));
    TD_ASSERT(TD_OK == td_user_append(account_id, distinct_id, array_properties, ta));

    TD_ASSERT(TD_OK == td_user_uniq_append(account_id, distinct_id, array_properties, ta));

    td_free_properties(array_properties);

    td_flush(ta);
    td_free(ta);
    td_consumer_free(consumer);

    return 0;
}

TDProperties *dynamic_properties_func(void) {
    TDProperties *properties = td_init_properties();
    TDProperties *json_super_dyld = td_init_custom_properties("json_super_dyld");
    TD_ASSERT(TD_OK == td_add_date("dynamic", time(NULL), 0, properties));
    TD_ASSERT(TD_OK == td_add_string("a3", "b3", strlen("b3"), json_super_dyld));
    TD_ASSERT(TD_OK == td_add_string("product_name", "card", strlen("card"), json_super_dyld));
    TD_ASSERT(TD_OK == td_add_number("price", 30.989, json_super_dyld));
    TD_ASSERT(TD_OK == td_add_int("coin", -30, json_super_dyld));
    TD_ASSERT(TD_OK == td_add_string("order_id", "abc_123", strlen("abc_123"), json_super_dyld));
    TD_ASSERT(TD_OK == td_add_date("login_time", time(NULL), 0, json_super_dyld));
    TD_ASSERT(TD_OK == td_add_bool("is_firstBuy", TD_TRUE, json_super_dyld));
    TD_ASSERT(TD_OK == td_add_bool("is_test", TD_FALSE, json_super_dyld));
    TD_ASSERT(TD_OK == td_add_property(json_super_dyld, properties));
    return properties;
}

struct TDConsumer* getDebugConsumer(void) {
    char* serverUrl = "serverUrl";
    char* appId = "appId";

    TDConfig* config = td_init_config();
    struct TDConsumer* consumer = NULL;

    TD_ASSERT(TD_OK == td_add_string("device_id", "123", strlen("123"), config));
    TD_ASSERT(TD_OK == td_add_int("timeout", 60, config));
    /* debug_mode, 0 report to TE, 1 is not. */
    TD_ASSERT(TD_OK == td_add_int("debug_mode", 1, config));
    TD_ASSERT(TD_OK == td_add_int("log", 1, config));
    TD_ASSERT(TD_OK == td_add_string("push_url", serverUrl, strlen(serverUrl), config));
    TD_ASSERT(TD_OK == td_add_string("appid", appId, strlen(appId), config));

    if (TD_OK != td_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    td_free_properties(config);
    return consumer;
}

struct TDConsumer* getLoggingConsumer(void) {
    struct TDConsumer* consumer = NULL;
    TDConfig* config = td_init_config();
    char* logPath = "D:/log/";
    TD_ASSERT(TD_OK == td_add_string("file_path", logPath, strlen(logPath), config));
    TD_ASSERT(TD_OK == td_add_int("rotate_mode", TD_DAILY, config));
    TD_ASSERT(TD_OK == td_add_int("file_size", 1024, config));
    TD_ASSERT(TD_OK == td_add_bool("log", TD_TRUE, config));

    if (TD_OK != td_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    td_free_properties(config);
    return consumer;
}

struct TDConsumer* getBatchConsumer(void) {
    char* serverUrl = "serverUrl";
    char* appId = "appId";

    TDConfig* config = td_init_config();
    struct TDConsumer* consumer = NULL;

    TD_ASSERT(TD_OK == td_add_string("push_url", serverUrl, strlen(serverUrl), config));
    TD_ASSERT(TD_OK == td_add_string("appid", appId, strlen(appId), config));
    TD_ASSERT(TD_OK == td_add_int("batch_size", 10, config));

    if (TD_OK != td_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    td_free_properties(config);
    return consumer;
}

struct TDConsumer* getAsyncBatchConsumer(void) {
    char* serverUrl = "serverUrl";
    char* appId = "appId";

    TDConfig* config = td_init_config();
    struct TDConsumer* consumer = NULL;

    TD_ASSERT(TD_OK == td_add_int("log", 1, config));
    TD_ASSERT(TD_OK == td_add_string("push_url", serverUrl, strlen(serverUrl), config));
    TD_ASSERT(TD_OK == td_add_string("appid", appId, strlen(appId), config));
    TD_ASSERT(TD_OK == td_add_int("batch_size", 10, config));

    if (TD_OK != td_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
    }
    td_free_properties(config);
    return consumer;
}
