# ThinkingData C SDK

### 使用CMake生成项目文件，默认情况会生成thinkingdata库和thinkingdata_http库，thinkingdata写入本地文件，thinkingdata_http直接发送到服务器
### 可以根据实际情况进行使用，当使用thinkingdata_http时，需要将thirdparty/curl/lib/libcurl.dll文件引入到项目中

#### Debug build
```shell
mkdir cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug cmake-build-debug
```

#### Release build
```shell
mkdir cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release cmake-build-release
```

### API描述
#### 0.初始化:
```c
    //logging_consumer
    TAConfig* config = ta_init_properties();

    //配置日志路径 YOUR_LOG_PATH 需要更改为日志存储的目录
    TA_ASSERT(TA_OK == ta_add_string("file_path", "YOUR_LOG_PATH", strlen("YOUR_LOG_PATH"), config)); 
    
    //按时间切分 默认按小时切分
    //TA_ASSERT(TA_OK == ta_add_int("rotate_mode", DAILY, config)); //按天切分
    TA_ASSERT(TA_OK == ta_add_int("rotate_mode", HOURLY, config)); //按小时切分
    
    //按文件大小切分 
    //TA_ASSERT(TA_OK == ta_add_int("file_size", 1024, config));
    
    TAConsumer* consumer = NULL;
    if (TA_OK != ta_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
        return 1;
    }
    ta_free_properties(config);

    ThinkingdataAnalytics *ta = NULL;
    if (TA_OK != ta_init(consumer, &ta)) {
        fprintf(stderr, "Failed to initialize the SDK.");
        return 1;
    }
    
    //batch_consumer
    TAConfig* config = ta_init_properties();

    //配置接收地址
    TA_ASSERT(TA_OK == ta_add_string("push_url", "RECEIVER_URL", strlen("RECEIVER_URL"), config));
    
    //配置appid
    TA_ASSERT(TA_OK == ta_add_string("appid", "YOUR_APPID", strlen("YOUR_APPID"),config));
    
    //配置每批上传数据量，默认20
    TA_ASSERT(TA_OK == ta_add_int("batch_size", 10, config));

    //配置最大缓存批次量，默认50
    TA_ASSERT(TA_OK == ta_add_int("max_cache_size", 50, config));
    
    //配置超时时间，单位秒，默认30
    TA_ASSERT(TA_OK == ta_add_int("timeout", 30, config));

    TAConsumer* consumer = NULL;
    if (TA_OK != ta_init_consumer(&consumer, config)) {
        fprintf(stderr, "Failed to initialize the consumer.");
        return 1;
    }
    ta_free_properties(config);

    ThinkingdataAnalytics *ta = NULL;
    if (TA_OK != ta_init(consumer, &ta)) {
        fprintf(stderr, "Failed to initialize the SDK.");
        return 1;
    }
```

文件默认以小时切分，需要搭配 LogBus 进行上传
YOUR_LOG_PATH 需要更改为日志存储的目录，您只需将 LogBus 的监听文件夹地址设置为此处的地址，即可使用 LogBus 进行数据的监听上传。

#### 1.track 方法：
```c
    const char* distinct_id = "ABC123";
    const char* account_id = "TA_10001";
    
    TA_ASSERT(TA_OK == ta_track(NULL, distinct_id, "test", NULL, ta));

```
注意：track接口account_id 和 distinct_id 必须有其一。

#### 2.track 带属性方法：
```c  
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
    TA_ASSERT(TA_OK == ta_add_bool("is_firstBuy", true, properties));
    TA_ASSERT(TA_OK == ta_add_bool("is_test", false, properties));

    //埋点
    TA_ASSERT(TA_OK == ta_track(account_id, distinct_id, "test", properties, ta));
    ta_free_properties(properties);
```
* 事件的名称是string类型，只能以字母开头，可包含数字，字母和下划线 “\_”，长度最大为 50 个字符，对字母大小写不敏感。
* 事件的属性类型是TAProperties,可以通过ta_add_string添加字符串型属性，ta_add_number添加数值型属性，ta_add_int添加int属性，ta_add_bool添加bool值，ta_add_date添加date类型值。
* Key 的值为属性的名称，为string类型，规定只能以字母开头，包含数字，字母和下划线 “\_”，长度最大为 50 个字符，对字母大小写不敏感。  
* Value 为该属性的值，可以是int、number、string、bool、date类型

#### 3.公共属性相关接口
```c    
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
    
```

#### 4.user相关接口：
```c  
    TAProperties* user_properties = ta_init_properties();
    TA_ASSERT(TA_OK == ta_add_int("Level", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_set(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_add(account_id, distinct_id, user_properties, ta));
    TA_ASSERT(TA_OK == ta_user_del(account_id, distinct_id, ta));
    TA_ASSERT(TA_OK == ta_add_int("Age", 3, user_properties));
    TA_ASSERT(TA_OK == ta_user_setOnce(account_id, distinct_id, user_properties,ta));
    ta_free_properties(user_properties);
```

注意：ta_user_add接口属性值只允许传入int、number类型。

#### 5.flush 接口：
```c  
    ta_flush(ta);
```

#### 6.free 方法：
```c  
    ta_free(ta);
    ta_consumer_free(consumer);
```