#include <iostream>
#include <string.h>

#include <thinkingdata.h>

using namespace std;

int main()
{
     TAConfig* config = ta_init_properties();
    
    TA_ASSERT(TA_OK == ta_add_string("file_path", "/Users/sunyujuan/Downloads/c_sdk_test_log", strlen("/Users/sunyujuan/Downloads/c_sdk_test_log"), config)); //配置日志路径
    
    //按时间切分 默认按小时切分
    //TA_ASSERT(TA_OK == ta_add_int("rotate_mode", DAILY, config)); //按天切分
    TA_ASSERT(TA_OK == ta_add_int("rotate_mode", HOURLY, config)); //按小时切分
    
    //按文件大小切分 
//    TA_ASSERT(TA_OK == ta_add_int("file_size", 1024, config));
    
    TALoggingConsumer* consumer = NULL;
    if (TA_OK != ta_init_logging_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
        return 1;
    }
    ta_free_properties(config);

    ThinkingdataAnalytics *ta = NULL;
    if (TA_OK != ta_init(consumer, &ta)) {
        fprintf(stderr, "Failed to initialize the SDK.");
        return 1;
    }

    const char* distinct_id = "ABC123";
    const char* account_id = "TA_10001";
    
    TA_ASSERT(TA_OK == ta_track(NULL, distinct_id, "test", NULL, ta));
    
    TAProperties* properties = ta_init_properties();
    // 添加属性

    // 设置事件发生的时间，如果不设置的话，则默认使用为当前时间，**注意** #time的类型必须是time_t
    TA_ASSERT(TA_OK == ta_add_date("#time", time(NULL), 0, properties));

    // 设置用户的ip地址，TA系统会根据IP地址解析用户的地理位置信息，如果不设置的话，则默认不上报
    TA_ASSERT(TA_OK == ta_add_string("#ip", "192.168.1.1", strlen("192.168.1.1"), properties));

    TA_ASSERT(TA_OK == ta_add_string("product_name", "月卡", strlen("月卡"), properties));
    TA_ASSERT(TA_OK == ta_add_number("price", 30.989, properties));
    TA_ASSERT(TA_OK == ta_add_int("coin", -30, properties));
    TA_ASSERT(TA_OK == ta_add_string("order_id", "abc_123", strlen("abc_123"), properties));
    TA_ASSERT(TA_OK == ta_add_date("login_time", time(NULL), 0, properties));
    TA_ASSERT(TA_OK == ta_add_bool("is_firstBuy", TA_TRUE, properties));
    TA_ASSERT(TA_OK == ta_add_bool("is_test", TA_FALSE, properties));

    //埋点
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test", properties, ta));
    ta_free_properties(properties);
    
    //公共属性操作
    TAProperties* super_properties = ta_init_properties();
    TA_ASSERT(TA_OK == ta_add_string("super_property_key", "super_property_value", strlen("super_property_value"), super_properties));
    TA_ASSERT(TA_OK == ta_add_string("super_property_key2", "super_property_value2", strlen("super_property_value"), super_properties));
    ta_set_super_properties(super_properties, ta);
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test_super", NULL, ta));

    ta_unset_super_properties("super_property_key2", ta);
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test_super2", NULL, ta));
    
    ta_clear_super_properties(ta);
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test_super3", NULL, ta));
    ta_free_properties(super_properties);
    
    TAProperties* user_properties = ta_init_properties();
    TA_ASSERT(TA_OK == ta_add_int("Level", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_set(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_add(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_del(account_id, distinct_id, ta));
    TA_ASSERT(TA_OK == ta_add_int("Age", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_setOnce(account_id, distinct_id, user_properties, ta));
    ta_free_properties(user_properties);
    
    ta_flush(ta);
    ta_free(ta);
    ta_consumer_free(consumer);
    return 0;
}