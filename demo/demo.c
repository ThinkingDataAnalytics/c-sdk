#include <string.h>

#include <thinkingdata.h>

TAProperties *dynamic_properties_func(void);

int main(int args, char **argv) {
    
    TAConfig *config = ta_init_config();
    struct TAConsumer *consumer = NULL;
    ThinkingdataAnalytics *ta = NULL;
    const char *distinct_id = "ABC123";
    const char *account_id = "TA_10001";
    TAProperties *properties = ta_init_properties();
    TAProperties *super_properties = ta_init_properties();
    TAProperties *user_properties = ta_init_properties();
    TAProperties *array_properties = ta_init_properties();

    (void) (args);
    (void) (argv);

    TA_ASSERT(TA_OK == ta_add_string("push_url", "https://{RECEIVER_URL}", strlen("https://{RECEIVER_URL}"), config));
    TA_ASSERT(TA_OK == ta_add_string("appid", "YOUR_APPID", strlen("YOUR_APPID"), config));
	TA_ASSERT(TA_OK == ta_add_int("batch_size", 10, config));

    /*TA_ASSERT(TA_OK == ta_add_int("rotate_mode", DAILY, config));*/
//    TA_ASSERT(TA_OK == ta_add_int("rotate_mode", HOURLY, config));

/*    TA_ASSERT(TA_OK == ta_add_int("file_size", 1024, config));
    TA_ASSERT(TA_OK == ta_add_bool("log", TA_TRUE, config));*/

    if (TA_OK != ta_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
        return 1;
    }
    ta_free_properties(config);

	
    if (TA_OK != ta_init(consumer, &ta)) {
        fprintf(stderr, "Failed to initialize the SDK.");
        return 1;
    }

	
    ta_set_dynamic_properties(&dynamic_properties_func, ta);
	
    TA_ASSERT(TA_OK == ta_track(NULL, distinct_id, "test", NULL, ta));

    TA_ASSERT(TA_OK == ta_add_date("#time", time(NULL), 0, properties));
    TA_ASSERT(TA_OK == ta_add_string("#ip", "192.168.1.1", strlen("192.168.1.1"), properties));
    TA_ASSERT(TA_OK == ta_add_string("#first_check_id", "test_1", strlen("test_1"), properties));

    TA_ASSERT(TA_OK == ta_add_string("product_name", "月卡", strlen("月卡"), properties));
    TA_ASSERT(TA_OK == ta_add_number("price", 30.989, properties));
    TA_ASSERT(TA_OK == ta_add_int("coin", -30, properties));
    TA_ASSERT(TA_OK == ta_add_string("order_id", "abc_123", strlen("abc_123"), properties));
    TA_ASSERT(TA_OK == ta_add_date("login_time", time(NULL), 0, properties));
    TA_ASSERT(TA_OK == ta_add_bool("is_firstBuy", TA_TRUE, properties));
    TA_ASSERT(TA_OK == ta_add_bool("is_test", TA_FALSE, properties));

    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name1", strlen("product_name1"), properties));
    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name2", strlen("product_name2"), properties));

    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test", properties, ta));
    ta_free_properties(properties);

    TA_ASSERT(TA_OK == ta_add_string("super_property_key", "super_property_value", strlen("super_property_value"),
                                     super_properties));
    TA_ASSERT(TA_OK == ta_add_string("super_property_key2", "super_property_value2", strlen("super_property_value2"),
                                     super_properties));
    ta_set_super_properties(super_properties, ta);
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test_super", NULL, ta));

    ta_unset_super_properties("super_property_key2", ta);
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test_super2", NULL, ta));

    ta_clear_super_properties(ta);
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test_super3", NULL, ta));
    ta_free_properties(super_properties);

    TA_ASSERT(TA_OK == ta_add_int("Level", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_set(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_add(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_del(account_id, distinct_id, ta));
    TA_ASSERT(TA_OK == ta_add_int("Age", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_setOnce(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_unset(account_id, distinct_id, "test", ta));

    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name3", strlen("product_name3"), array_properties));
    TA_ASSERT(TA_OK == ta_append_array("product_buy", "product_name4", strlen("product_name4"), array_properties));
    TA_ASSERT(TA_OK == ta_user_append(account_id, distinct_id, array_properties, ta));
    ta_free_properties(array_properties);

    ta_free_properties(user_properties);
    ta_flush(ta);
    ta_free(ta);
    ta_consumer_free(consumer);

    return 0;
}

TAProperties *dynamic_properties_func() {
    TAProperties *properties = ta_init_properties();
    TA_ASSERT(TA_OK == ta_add_date("dynamic", time(NULL), 0, properties));
    return properties;
}
