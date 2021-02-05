#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "json.h"
#include "util.h"
#include "list.h"

#if defined(USE_POSIX)
  #if defined(_WIN32)
    #error Both USE_POSIX and _WIN32 are defined
  #endif
  #include <pthread.h>
  #include <regex.h>
  #include <sys/time.h>
#elif defined(_WIN32)
  #include <windows.h>
  #include <sys/timeb.h>
  #include "pcre/pcre.h"
#else
  #error Neither USE_POSIX nor _WIN32 is defined
#endif

#include "thinkingdata_private.h"
#include "thinkingdata.h"

#define TA_LIB_VERSION "1.2.0"
#define TA_LIB "C"

#define NAME_PATTERN "^[a-zA-Z#][a-zA-Z0-9_]{0,49}$"

const char TA_DATA_KEY_TIME[]         = "#time";
const char TA_DATA_KEY_IP[]           = "#ip";
const char TA_DATA_KEY_UUID[]         = "#uuid";
const char TA_DATA_KEY_TYPE[]         = "#type";
const char TA_DATA_KEY_ACCOUNT_ID[]   = "#account_id";
const char TA_DATA_KEY_DISTINCT_ID[]  = "#distinct_id";
const char TA_DATA_KEY_EVENT_NAME[]   = "#event_name";
const char TA_DATA_KEY_EVENT_ID[]     = "#event_id";
const char TA_DATA_FIRST_CHECK_ID[]   = "#first_check_id";
const char TA_DATA_KEY_LIB[]          = "#lib";
const char TA_DATA_KEY_LIB_VERSION[]  = "#lib_version";

const char TA_CONFIG_ROTATE_MODE[]    = "rotate_mode";
const char TA_CONFIG_FILE_SIZE[]      = "file_size";
const char TA_CONFIG_FILE_PATH[]      = "file_path";
const char TA_CONFIG_FILE_PREFIX[]    = "file_prefix";
const char TA_CONFIG_LOG[]            = "log";

typedef enum trackingType {
    TA_TRACK = 0,
    TA_TRACK_UPDATE,
    TA_TRACK_OVERWRITE,
    TA_USER_SET,
    TA_USER_SETONCE,
    TA_USER_UNSET,
    TA_USER_DEL,
    TA_USER_ADD,
    TA_USER_APPEND,

    TA_TYPES_NUM
} tracking_type_t;

static inline void ta_type_names(char *types[]){
    types[TA_TRACK]           = "track";
    types[TA_TRACK_UPDATE]    = "track_update";
    types[TA_TRACK_OVERWRITE] = "track_overwrite";
    types[TA_USER_SET]        = "user_set";
    types[TA_USER_SETONCE]    = "user_setOnce";
    types[TA_USER_UNSET]      = "user_unset";
    types[TA_USER_DEL]        = "user_del";
    types[TA_USER_ADD]        = "user_add";
    types[TA_USER_APPEND]     = "user_append";
}

#define out_of_memory(l, n) do {                    \
fprintf(stderr, "[%ld] Out of memory(%lu bytes).\n", l, (unsigned long)n);    \
exit(TA_MALLOC_ERROR);                     \
} while (0)

void *ta_safe_malloc(unsigned long n, unsigned long line) {
    void *ptr = malloc(n);
    if (ptr == NULL) {
        out_of_memory(line, n);
    }
    return ptr;
}

void ta_safe_free(void *curr, __attribute__((unused)) unsigned long line) {
    if (curr) {
        free(curr);
        curr = NULL;
    }
}

TAConfig *ta_init_config() {
    return ta_init_dict_node("config");
}

TAProperties *ta_init_properties() {
    return ta_init_dict_node("properties");
}

void ta_free_properties(TAProperties *properties) {
    ta_free_node(properties);
}

int ta_add_bool(const char *key, TABool bool_, TAProperties *properties) {
    if (NULL == properties) {
        fprintf(stderr, "Parameter 'properties' is NULL.");
        return TA_INVALID_PARAMETER_ERROR;
    }
    TANodeValue value;
    value.boolean_ = bool_;
    TANode *node_new = creat_new_node(TA_Boolean, key, &value);
    ta_add_node(node_new, properties);
    return TA_OK;
}

int ta_add_number(const char *key, double number_, TAProperties *properties) {
    if (NULL == properties) {
        fprintf(stderr, "Parameter 'properties' is NULL.");
        return TA_INVALID_PARAMETER_ERROR;
    }
    TANodeValue value;
    value.number_ = number_;
    TANode *node_new = creat_new_node(TA_NUMBER, key, &value);
    ta_add_node(node_new, properties);
    return TA_OK;
}

int ta_add_int(const char *key, long long int_, TAProperties *properties) {
    if (NULL == properties) {
        fprintf(stderr, "Parameter 'properties' is NULL.");
        return TA_INVALID_PARAMETER_ERROR;
    }
    TANodeValue value;
    value.int_ = int_;
    TANode *node_new = creat_new_node(TA_INT, key, &value);
    ta_add_node(node_new, properties);
    return TA_OK;
}

int ta_add_date(const char *key, time_t seconds, int microseconds, TAProperties *properties) {
    if (NULL == properties) {
        fprintf(stderr, "Parameter 'properties' is NULL.");
        return TA_INVALID_PARAMETER_ERROR;
    }

    TANodeValue value;
    value.date_.seconds = seconds;
    value.date_.microseconds = microseconds;
    TANode *node_new = creat_new_node(TA_DATE, key, &value);
    ta_add_node(node_new, properties);
    return TA_OK;
}

int ta_add_string(const char *key, const char *string_, unsigned int length, TAProperties *properties) {
    if (NULL == properties) {
        fprintf(stderr, "Parameter 'properties' is NULL.");
        return TA_INVALID_PARAMETER_ERROR;
    }

    TANodeValue value;
    value.string_ = (char *) TA_SAFE_MALLOC(length + 1);
    memcpy(value.string_, string_, length);
    value.string_[length] = 0;
    TANode *node_new = creat_new_node(TA_STRING, key, &value);
    ta_add_node(node_new, properties);
    return TA_OK;
}

int ta_append_array(const char *key, const char *string_, unsigned int length, TAProperties *properties) {
    if (NULL == properties) {
        fprintf(stderr, "Parameter 'properties' is NULL.");
        return TA_INVALID_PARAMETER_ERROR;
    }

    TANode *list = ta_find_node(key, properties);
    if (NULL == list) {
        list = ta_init_array_node(key);
        list->type = TA_ARRAY;
        if (NULL == list) {
            return TA_MALLOC_ERROR;
        }
        ta_add_node(list, properties);
    }

    TANodeValue value;
    value.string_ = (char *) TA_SAFE_MALLOC(length + 1);
    memcpy(value.string_, string_, length);
    value.string_[length] = 0;
    TANode *node_new = creat_new_node(TA_STRING, NULL, &value);
    if (NULL == node_new) {
        return TA_MALLOC_ERROR;
    }

    ta_add_node_copy(node_new, list);

    TA_SAFE_FREE(value.string_);
    TA_SAFE_FREE(node_new);

    return TA_OK;
}

typedef struct {
    int file_size;
    bool log;
    char last_file_date[512];
    int last_file_count;
    char file_path[1024];
    char file_prefix[64];
    TARotateMode rotate_mode;
    FILE *file;
} TALoggingConsumerInter;

static int ta_logging_consumer_flush(void *this_) {
    TALoggingConsumerInter *inter = (TALoggingConsumerInter *) this_;
    if (NULL != inter->file && 0 == fflush(inter->file)) {
        return TA_OK;
    }
    return TA_IO_ERROR;
}

static int ta_logging_consumer_close(void *this_) {
    if (NULL == this_) {
        return TA_INVALID_PARAMETER_ERROR;
    }

    TALoggingConsumerInter *inter = (TALoggingConsumerInter *) this_;
    if (NULL != inter->file) {
        fflush(inter->file);
        fclose(inter->file);
        inter->file = NULL;
        inter->last_file_count = 0;
    }

    return TA_OK;
}

static bool file_size(char *file_name, int file_size) {
    struct stat buffer;
    stat(file_name, &buffer);
    long size = buffer.st_size / (1024 * 1024);
    if (size >= file_size) {
        return true;
    } else {
        return false;
    }
}

static int ta_logging_consumer_add(void *this_, const char *event, unsigned long length) {
    if (NULL == this_ || NULL == event) {
        return TA_INVALID_PARAMETER_ERROR;
    }

    TALoggingConsumerInter *inter = (TALoggingConsumerInter *) this_;
    char *file_name_date = TA_SAFE_MALLOC(512);

    struct tm tm;
    time_t t = time(NULL);
    LOCALTIME(&t, &tm);

    char *file_prefix = NULL;
    int prefixLength = strlen(inter->file_prefix);
    if (prefixLength > 0) {
        file_prefix = (char *) malloc(prefixLength + 4);
        strcpy(file_prefix, inter->file_prefix);
        strcat(file_prefix, ".log");
    } else {
        file_prefix = (char *) malloc(3);
        strcpy(file_prefix, "log");
    }

    if (inter->rotate_mode == DAILY) {
        snprintf(file_name_date, 512, "%s.%4d-%02d-%02d_", file_prefix, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    } else if (inter->rotate_mode == HOURLY) { //默认按小时切分
        snprintf(file_name_date, 512, "%s.%4d-%02d-%02d-%02d_", file_prefix, tm.tm_year + 1900, tm.tm_mon + 1,
                 tm.tm_mday, tm.tm_hour);
    }
    free(file_prefix);

    int count = 0;
    bool need_new_file = false;
    if (strcmp(inter->last_file_date, file_name_date) == 0) {
        if (inter->file_size > 0) {
            count = inter->last_file_count;
            fseek(inter->file, 0, SEEK_END);
            long size = ftell(inter->file) / (1024 * 1024);
            if (size >= inter->file_size) {
                count++;
                need_new_file = true;
            }
        } else {
            need_new_file = false;
        }
    } else {
        need_new_file = true;
        snprintf(inter->last_file_date, 512, "%s", file_name_date);
    }

    if (need_new_file == true) {
        char *file_path = TA_SAFE_MALLOC(1024);
        ta_logging_consumer_close(this_);
        snprintf(file_path, 1024, "%s/%s%d", inter->file_path, file_name_date, count);

        if (inter->file_size > 0) {
            while (file_exists(file_path) == 0 && file_size(file_path, inter->file_size)) {
                count++;
                snprintf(file_path, 1024, "%s/%s%d", inter->file_path, file_name_date, count);
            }
        }
        FILE *file = NULL;
        unsigned int times = 0;
        while ((file = fopen(file_path, "a")) == NULL && times++ < 10) {
            fprintf(stderr, "Fail to open file %s: %s", file_path, strerror(errno));
            snprintf(file_path, 1024, "%s/%s%d", inter->file_path, file_name_date, ++count);
        }
        inter->last_file_count = count;
        inter->file = file;
        TA_SAFE_FREE(file_path);
    }

    fwrite(event, length, 1, inter->file);
    fwrite("\n", 1, 1, inter->file);

    TA_SAFE_FREE(file_name_date);
    return TA_OK;
}

static void get_config_param(TALoggingConsumerInter *inter, const TAConfig *config) {
    TAListNode *curr = config->value.child;
    while (NULL != curr) {
        if (NULL != curr->value->key) {
            if (0 == strncmp(TA_CONFIG_ROTATE_MODE, curr->value->key, 256)) {
                TANode *rotate_node = curr->value;
                if (NULL != rotate_node) {
                    inter->rotate_mode = (TARotateMode) rotate_node->value.int_;
                }
            } else if (0 == strncmp(TA_CONFIG_FILE_SIZE, curr->value->key, 256)) {
                TANode *file_size_node = curr->value;
                if (NULL != file_size_node) {
                    inter->file_size = (int) file_size_node->value.int_;
                }
            } else if (0 == strncmp(TA_CONFIG_FILE_PATH, curr->value->key, 256)) {
                TANode *file_path_node = curr->value;
                if (NULL != file_path_node) {
                    if (file_exists(file_path_node->value.string_) != 0) {
                        mkdirs(file_path_node->value.string_);
                    }
                    snprintf(inter->file_path, 1024, "%s", file_path_node->value.string_);
                }
            } else if (0 == strncmp(TA_CONFIG_FILE_PREFIX, curr->value->key, 256)) {
                TANode *file_prefix = curr->value;
                if (NULL != file_prefix) {
                    snprintf(inter->file_prefix, 64, "%s", file_prefix->value.string_);
                }
            } else if (0 == strncmp(TA_CONFIG_LOG, curr->value->key, 256)) {
                TANode *log_node = curr->value;
                if (NULL != log_node) {
                    inter->log = (bool) log_node->value.boolean_;
                }
            }
        }
        curr = curr->next;
    }
}

int ta_init_logging_consumer(TALoggingConsumer **ta, const TAConfig *config) {
    TALoggingConsumerInter *inter = (TALoggingConsumerInter *) TA_SAFE_MALLOC(sizeof(TALoggingConsumerInter));
    memset(inter, 0, sizeof(TALoggingConsumerInter));

    inter->last_file_count = 0;
    if (config != NULL) {
        get_config_param(inter, config);
    }

    *ta = (TALoggingConsumer *) TA_SAFE_MALLOC(sizeof(TALoggingConsumer));
    memset(*ta, 0, sizeof(TALoggingConsumer));

    (*ta)->this_ = (void *) inter;
    (*ta)->op.add = &ta_logging_consumer_add;
    (*ta)->op.flush = &ta_logging_consumer_flush;
    (*ta)->op.close = &ta_logging_consumer_close;

    return TA_OK;
}

struct ThinkingdataAnalytics {
    TAProperties *super_properties;
    ta_dynamic_func dynamic_properties;
#if defined(USE_POSIX)
    pthread_mutex_t mutex;
    regex_t regex;
#elif defined(_WIN32)
    CRITICAL_SECTION mutex;
    pcre *regex;
#endif
    struct TAConsumer *consumer;
    char *event_types[TA_TYPES_NUM];
};

int ta_init(struct TAConsumer *consumer, ThinkingdataAnalytics **tracker) {
    *tracker = (ThinkingdataAnalytics *) TA_SAFE_MALLOC(sizeof(ThinkingdataAnalytics));
    memset(*tracker, 0, sizeof(ThinkingdataAnalytics));

    (*tracker)->consumer = consumer;
    (*tracker)->super_properties = ta_init_properties();
    ta_type_names((*tracker)->event_types);

#if defined(USE_POSIX)
    if (pthread_mutex_init(&((*tracker)->mutex), NULL) != 0) {
        fprintf(stderr, "Initialize mutex error.");
        return TA_MALLOC_ERROR;
    }

    if (0 != regcomp(&((*tracker)->regex), NAME_PATTERN, REG_EXTENDED | REG_ICASE | REG_NOSUB)) {
        fprintf(stderr, "Compile regex error.");
        return TA_MALLOC_ERROR;
    }
#elif defined(_WIN32)
    InitializeCriticalSection(&((*tracker)->mutex));

    const char *error_message = NULL;
    int offset = -1;
    if (NULL == ((*tracker)->regex = pcre_compile(NAME_PATTERN, PCRE_EXTENDED | PCRE_CASELESS, &error_message, &offset, NULL))) {
        fprintf(stderr, "Compile regex error. ErrMsg:%s, Offset:%d", error_message, offset);
        return TA_MALLOC_ERROR;
    }
#endif

    return TA_OK;
}

void ta_consumer_free(TALoggingConsumer *cosumer) {
    if (NULL == cosumer) {
        return;
    }

    TA_SAFE_FREE(cosumer->this_);
    TA_SAFE_FREE(cosumer);
}

void ta_free(ThinkingdataAnalytics *ta) {
    if (NULL == ta) {
        return;
    }

    ta_free_properties(ta->super_properties);

#if defined(USE_POSIX)
    pthread_mutex_destroy(&(ta->mutex));
    regfree(&(ta->regex));
#elif defined(_WIN32)
    DeleteCriticalSection(&(ta->mutex));
    pcre_free(ta->regex);
#endif

    ta->consumer->op.close(ta->consumer->this_);

    TA_SAFE_FREE(ta);
}

void ta_flush(ThinkingdataAnalytics *ta) {
    ta->consumer->op.flush(ta->consumer->this_);
}

// 设置公共事件属性
int ta_set_super_properties(const TAProperties *properties, ThinkingdataAnalytics *ta) {
    if (NULL == ta || NULL == properties || TA_DICT != properties->type) {
        return TA_INVALID_PARAMETER_ERROR;
    }

#if defined(USE_POSIX)
    pthread_mutex_lock(&ta->mutex);
#elif defined(_WIN32)
    EnterCriticalSection(&ta->mutex);
#endif
    struct TAListNode *curr = properties->value.child;
    while (NULL != curr) {
        ta_add_node_copy(curr->value, ta->super_properties);
        curr = curr->next;
    }
#if defined(USE_POSIX)
    pthread_mutex_unlock(&ta->mutex);
#elif defined(_WIN32)
    LeaveCriticalSection(&ta->mutex);
#endif

    return TA_OK;
}

// 清除公共事件属性
int ta_unset_super_properties(const char *key, ThinkingdataAnalytics *ta) {
#if defined(USE_POSIX)
    pthread_mutex_lock(&ta->mutex);
#elif defined(_WIN32)
    EnterCriticalSection(&ta->mutex);
#endif
    ta_delete_node(key, ta->super_properties);
#if defined(USE_POSIX)
    pthread_mutex_unlock(&ta->mutex);
#elif defined(_WIN32)
    LeaveCriticalSection(&ta->mutex);
#endif
    return TA_OK;
}

// 清空公共事件属性
int ta_clear_super_properties(ThinkingdataAnalytics *ta) {
#if defined(USE_POSIX)
    pthread_mutex_lock(&ta->mutex);
#elif defined(_WIN32)
    EnterCriticalSection(&ta->mutex);
#endif
    ta_remove_node(ta->super_properties);
    ta->super_properties->value.child = NULL;
#if defined(USE_POSIX)
    pthread_mutex_unlock(&ta->mutex);
#elif defined(_WIN32)
    LeaveCriticalSection(&ta->mutex);
#endif
    return TA_OK;
}

void ta_set_dynamic_properties(ta_dynamic_func func, ThinkingdataAnalytics *ta) {
    ta->dynamic_properties = func;
}

#if defined(USE_POSIX)
static int ta_assert_key_name(const char *key, regex_t regex) {
#elif defined(_WIN32)
static int ta_assert_key_name(const char *key, pcre *regex) {
#else
static int ta_assert_key_name(const char *key) {
#endif
    unsigned long key_len = (NULL == key ? (unsigned long) -1 : strlen(key));
    if (key_len < 1 || key_len > 255) {
        return TA_INVALID_PARAMETER_ERROR;
    }
#if defined(USE_POSIX)
    if (0 != regexec(&regex, key, 0, NULL, 0)) {
        return TA_INVALID_PARAMETER_ERROR;
    }
#elif defined(_WIN32)
    if (0 > pcre_exec(regex, NULL, key, strlen(key), 0, 0, NULL, 0)) {
        return TA_INVALID_PARAMETER_ERROR;
    }

#endif
    return TA_OK;
}

static int ta_check_parameter(const char *account_id,
                              const char *distinct_id,
                              TANodeType type,
                              const char *event,
                              const struct TANode *properties,
                              ThinkingdataAnalytics *ta) {
    unsigned long account_id_len = (NULL == account_id ? (unsigned long) -1 : strlen(account_id));
    unsigned long distinct_id_len = (NULL == distinct_id ? (unsigned long) -1 : strlen(distinct_id));
    if ((NULL == account_id || account_id_len < 1) && (NULL == distinct_id || distinct_id_len < 1)) {
        fprintf(stderr, "Must have account id or distinct id\n");
        return TA_INVALID_PARAMETER_ERROR;
    }

    if (NULL != distinct_id && distinct_id_len > 255) {
        fprintf(stderr, "Invalid distinct id [%s].\n", distinct_id);
        return TA_INVALID_PARAMETER_ERROR;
    }
    if (NULL != account_id && account_id_len > 255) {
        fprintf(stderr, "Invalid account id [%s].\n", account_id);
        return TA_INVALID_PARAMETER_ERROR;
    }

    if (type == TA_TRACK &&
        (NULL == event
         #if defined(USE_POSIX)
         || TA_OK != ta_assert_key_name(event, ta->regex))) {
         #elif defined(_WIN32)
         || TA_OK != ta_assert_key_name(event, ta->regex))) {
         #else
         || TA_OK != ta_assert_key_name(event))) {
#endif
        fprintf(stderr, "Invalid event name [%s].\n", event == NULL ? "NULL" : event);
        return TA_INVALID_PARAMETER_ERROR;
    }

    if (NULL != properties) {
        TAListNode *curr = properties->value.child;
        while (NULL != curr) {
            if (NULL == curr->value->key
                #if defined(USE_POSIX)
                || TA_OK != ta_assert_key_name(curr->value->key, ta->regex)) {
                #elif defined(_WIN32)
                || TA_OK != ta_assert_key_name(curr->value->key, ta->regex)) {
                #else
                || TA_OK != ta_assert_key_name(curr->value->key)) {
                #endif
                fprintf(stderr, "Invalid property name [%s].\n", NULL == curr->value->key ? "NULL" : curr->value->key);
                return TA_INVALID_PARAMETER_ERROR;
            }
            if (type == TA_USER_ADD) {
                if (curr->value->type != TA_NUMBER && curr->value->type != TA_INT) {
                    fprintf(stderr, "user_add only support Number\n");
                    return TA_INVALID_PARAMETER_ERROR;
                }
            }
            curr = curr->next;
        }
    }
    return TA_OK;
}

static int analysis_properties(const struct TANode *properties, TANode *data, TANode *final_properties) {
    int res = TA_OK;
    TAListNode *curr = properties->value.child;
    while (NULL != curr) {
        if (NULL != curr->value->key && 0 == strncmp(TA_DATA_KEY_TIME, curr->value->key, 256)) {
            TANode *time_node = curr->value;
            if (NULL != time_node) {
                char *buf = convert_time_to_string(time_node);
                if (TA_OK != (res = ta_add_string(TA_DATA_KEY_TIME, buf, 64, data))) {
                    TA_SAFE_FREE(buf);
                    return res;
                } else {
                    TA_SAFE_FREE(buf);
                }
            }
        } else if (NULL != curr->value->key && 0 == strncmp(TA_DATA_KEY_IP, curr->value->key, 256)) {
            TANode *ip_node = curr->value;
            if (NULL != ip_node) {
                if (TA_OK !=
                    (res = ta_add_string(TA_DATA_KEY_IP, ip_node->value.string_, (int) strlen(ip_node->value.string_),
                                         data))) {
                    return res;
                }
            }
        } else if (NULL != curr->value->key && 0 == strncmp(TA_DATA_KEY_UUID, curr->value->key, 256)) {
            TANode *uuid_node = curr->value;
            if (NULL != uuid_node) {
                if (TA_OK != (res = ta_add_string(TA_DATA_KEY_UUID, uuid_node->value.string_,
                                                  (int) strlen(uuid_node->value.string_), data))) {
                    return res;
                }
            }
        } else {
            ta_add_node_copy(curr->value, final_properties);
        }
        curr = curr->next;
    }
    return TA_OK;
}

static inline char *get_tracking_type(ThinkingdataAnalytics *tracker, tracking_type_t type) {
    TA_ASSERT(NULL != tracker);
    return tracker->event_types[type];
}

static int ta_internal_track(const char *account_id,
                             const char *distinct_id,
                             tracking_type_t type,
                             const char *event,
                             const char *event_id,
                             const struct TANode *properties,
                             ThinkingdataAnalytics *ta) {
    int res = TA_OK;

    if (TA_OK != (res = ta_check_parameter(account_id, distinct_id, type, event, properties, ta))) {
        return res;
    }

    TANode *data = ta_init_dict_node("data");

    if (TA_OK !=
        (res = ta_add_string(TA_DATA_KEY_TYPE, get_tracking_type(ta, type), (int) strlen(get_tracking_type(ta, type)),
                             data))) {
        return res;
    }

    if (account_id &&
        (TA_OK != (res = ta_add_string(TA_DATA_KEY_ACCOUNT_ID, account_id, (int) strlen(account_id), data)))) {
        return res;
    }

    if (distinct_id &&
        (TA_OK != (res = ta_add_string(TA_DATA_KEY_DISTINCT_ID, distinct_id, (int) strlen(distinct_id), data)))) {
        return res;
    }

    int seconds;
    int microseconds = 0;

#if defined(USE_POSIX)
    struct timeval now;
    gettimeofday(&now, NULL);
    seconds = (int)now.tv_sec;
    microseconds = now.tv_usec / 1000;
#elif defined(_WIN32)
    struct timeb now;
    ftime(&now);
    seconds = now.time;
    microseconds = now.millitm;
#else
    time_t now = time(NULL);
    seconds = (int) now;
#endif
    if (TA_OK != (res = ta_add_date(TA_DATA_KEY_TIME, seconds, microseconds, data))) {
        return res;
    }

    TANode *final_properties = ta_init_dict_node("properties");
    if (type == TA_TRACK || type == TA_TRACK_UPDATE || type == TA_TRACK_OVERWRITE) {
        ta_add_string(TA_DATA_KEY_EVENT_NAME, event, (int)strlen(event), data);
        ta_add_string(TA_DATA_KEY_LIB, TA_LIB, strlen(TA_LIB), final_properties);
        ta_add_string(TA_DATA_KEY_LIB_VERSION, TA_LIB_VERSION, strlen(TA_LIB_VERSION), final_properties);
        
        if (type == TA_TRACK_UPDATE || type == TA_TRACK_OVERWRITE) {
            ta_add_string(TA_DATA_KEY_EVENT_ID, event_id, (int)strlen(event_id), data);
        } else if (NULL != event_id) {
            ta_add_string(TA_DATA_FIRST_CHECK_ID, event_id, (int)strlen(event_id), data);
        }
        
#if defined(USE_POSIX)
        pthread_mutex_lock(&ta->mutex);
#elif defined(_WIN32)
        EnterCriticalSection(&ta->mutex);
#endif
        TAListNode *curr = ta->super_properties->value.child;
        while (NULL != curr) {
            ta_add_node_copy(curr->value, final_properties);
            curr = curr->next;
        }

        if (ta->dynamic_properties) {
            TAProperties *dynamic = ta->dynamic_properties();
            TAListNode *node = dynamic->value.child;
            while (NULL != node) {
                ta_add_node_copy(node->value, final_properties);
                node = node->next;
            }
            ta_free_node(dynamic);
        }
#if defined(USE_POSIX)
        pthread_mutex_unlock(&ta->mutex);
#elif defined(_WIN32)
        LeaveCriticalSection(&ta->mutex);
#endif
    }

    if (NULL != properties) {
        if (TA_OK != (res = analysis_properties(properties, data, final_properties))) {
            return res;
        }
    }

    ta_add_node(final_properties, data);
    char *log_str = print_node(data, 0);

    TALoggingConsumerInter *inter = (TALoggingConsumerInter *) ta->consumer->this_;
    if (inter->log) {
        ta_debug("[ThinkingSDK] data:%s\n", log_str);
    }

#if defined(USE_POSIX)
    pthread_mutex_lock(&ta->mutex);
#elif defined(_WIN32)
    EnterCriticalSection(&ta->mutex);
#endif

    res = ta->consumer->op.add(inter, log_str, strlen(log_str));

#if defined(USE_POSIX)
    pthread_mutex_unlock(&ta->mutex);
#elif defined(_WIN32)
    LeaveCriticalSection(&ta->mutex);
#endif

    TA_SAFE_FREE(log_str);
    ta_free_node(data);

    return res;
}

int ta_track(const char *account_id,
             const char *distinct_id,
             const char *event,
             const TAProperties *properties,
             ThinkingdataAnalytics *ta) {
    return ta_internal_track(account_id,
                             distinct_id,
                             TA_TRACK,
                             event,
                             NULL,
                             properties,
                             ta);
}
        
int ta_track_update(const char *account_id,
                    const char *distinct_id,
                    const char *event,
                    const char *event_id,
                    const TAProperties *properties,
                    ThinkingdataAnalytics *ta) {
    return ta_internal_track(account_id,
                             distinct_id,
                             TA_TRACK_UPDATE,
                             event,
                             event_id,
                             properties,
                             ta);
}
        
int ta_track_overwrite(const char *account_id,
                       const char *distinct_id,
                       const char *event,
                       const char *event_id,
                       const TAProperties *properties,
                       ThinkingdataAnalytics *ta) {
    return ta_internal_track(account_id,
                             distinct_id,
                             TA_TRACK_OVERWRITE,
                             event,
                             event_id,
                             properties,
                             ta);
}

int ta_track_first_event(const char *account_id,
                         const char *distinct_id,
                         const char *event,
                         const char *firstCheckId,
                         const TAProperties *properties,
                         ThinkingdataAnalytics *ta) {
    return ta_internal_track(account_id,
                             distinct_id,
                             TA_TRACK,
                             event,
                             firstCheckId,
                             properties,
                             ta);
}

int ta_user_track(const char *account_id,
                  const char *distinct_id,
                  tracking_type_t type,
                  const TAProperties *properties,
                  ThinkingdataAnalytics *ta) {
    return ta_internal_track(account_id,
                             distinct_id,
                             type,
                             NULL,
                             NULL,
                             properties,
                             ta);
}

int ta_user_set(const char *account_id,
                const char *distinct_id,
                const TAProperties *properties,
                ThinkingdataAnalytics *ta) {
    if (NULL == properties) {
        return TA_INVALID_PARAMETER_ERROR;
    }
    return ta_user_track(account_id,
                         distinct_id,
                         TA_USER_SET,
                         properties,
                         ta);
}

int ta_user_unset(const char *account_id,
                  const char *distinct_id,
                  const char *propertyName,
                  ThinkingdataAnalytics *ta) {
    int res = TA_OK;
    TAProperties *properties = ta_init_properties();
    TA_ASSERT(TA_OK == ta_add_int(propertyName, 0, properties));
    res = ta_user_track(account_id,
                        distinct_id,
                        TA_USER_UNSET,
                        properties,
                        ta);
    ta_free_properties(properties);
    return res;
}

int ta_user_setOnce(const char *account_id,
                    const char *distinct_id,
                    const TAProperties *properties,
                    ThinkingdataAnalytics *ta) {
    if (NULL == properties) {
        return TA_INVALID_PARAMETER_ERROR;
    }
    return ta_user_track(account_id,
                         distinct_id,
                         TA_USER_SETONCE,
                         properties,
                         ta);
}

int ta_user_add(const char *account_id,
                const char *distinct_id,
                const TAProperties *properties,
                ThinkingdataAnalytics *ta) {
    if (NULL == properties) {
        return TA_INVALID_PARAMETER_ERROR;
    }
    return ta_user_track(account_id,
                         distinct_id,
                         TA_USER_ADD,
                         properties,
                         ta);
}

int ta_user_append(const char *account_id,
                   const char *distinct_id,
                   const TAProperties *properties,
                   ThinkingdataAnalytics *ta) {
    if (NULL == properties) {
        return TA_INVALID_PARAMETER_ERROR;
    }
    return ta_user_track(account_id,
                         distinct_id,
                         TA_USER_APPEND,
                         properties,
                         ta);
}

int ta_user_del(const char *account_id,
                const char *distinct_id,
                ThinkingdataAnalytics *ta) {
    int res = TA_OK;

    TAProperties *properties = ta_init_properties();
    res = ta_user_track(account_id,
                        distinct_id,
                        TA_USER_DEL,
                        properties,
                        ta);

    ta_free_properties(properties);

    return res;
}
