#include <string.h>
#include "thinkingdata_private.h"
#include "http_client.h"
#include "util.h"

const char TA_CONFIG_PUSH_URL[] = "push_url";
const char TA_CONFIG_APPID[] = "appid";
const char TA_CONFIG_TIMEOUT[] = "timeout";
const char TA_CONFIG_DEBUG_MODE[] = "debug_mode";


typedef struct TAData_ {
    int size;
    char *data;
    unsigned long body_size;
    struct TAData_ *next;
} TAData;

typedef struct {
    int size;
    TAData *head;
    TAData *end;
} TADataList;

typedef struct {
    int batch_size;
    TABool log;
    int timeout;
    int max_cache_size;
    char appid[50];
    char push_url[1024];
    TADataList data_list;
    TAData *current_data;
    TADebugMode debug_mode;
} TABatchConsumerInter;


static void remove_head(TADataList *data_list) {
    TAData *head = data_list->head;
    if (head == NULL) {
        return;
    }
    data_list->head = head->next;
    if (data_list->head == NULL) {
        data_list->end = NULL;
    }
    data_list->size--;
    TA_SAFE_FREE(head->data);
    TA_SAFE_FREE(head);
}

static int ta_batch_consumer_flush(void *this_) {
    TABatchConsumerInter *inter = (TABatchConsumerInter *) this_;
    HttpResponse *response;
    char *current_data_;
    TADataList *data_list = &inter->data_list;
    TAData *current_data = inter->current_data;
    if ((current_data != NULL && current_data->size >= inter->batch_size) || data_list->size <= 0) {
        if (current_data == NULL || current_data->size == 0) {
            return TA_OK;
        }

        current_data_ = (char *) TA_SAFE_REALLOC(current_data->data, current_data->body_size + 2);
        if (current_data_ == NULL) {
            return TA_MALLOC_ERROR;
        }
        current_data->data = current_data_;

        if (data_list->end == NULL) {
            data_list->head = current_data;
        } else {
            data_list->end->next = current_data;
        }
        data_list->end = current_data;
        inter->current_data = NULL;

        data_list->size++;
    }

    if (data_list->size > inter->max_cache_size) {
        // remove over sized data
        remove_head(data_list);
    }

    response = ta_debug_http_post(inter->appid, inter->push_url, current_data_, data_list->head->size,
                            strlen(current_data_), inter->timeout, inter->debug_mode);

    if (response != NULL) {
        if (inter->log) {
            ta_debug("response: status=%ld, body=%s\n", response->status, response->body);
        }
        if (response->status == 200) {
            remove_head(data_list);
            destroy_http_response(response);
            return TA_OK;
        } else {
            destroy_http_response(response);
        }
    }
    return TA_IO_ERROR;
}

static int ta_batch_consumer_close(void *this_) {
    TABatchConsumerInter *inter;
    int retryCount = 0;

    if (NULL == this_) {
        return TA_INVALID_PARAMETER_ERROR;
    }

    inter = (TABatchConsumerInter *) this_;
//    while (inter->data_list.size > 0 && inter->current_data->size > 0 && retryCount < 50) {
//        if (TA_OK == ta_batch_consumer_flush(inter)) {
//            retryCount = 0;
//        } else {
//            retryCount++;
//        }
//    }

    return TA_OK;
}

static int add_to_list(TABatchConsumerInter *inter, const char *event, unsigned long length) {
    TAData *current_data;
    char *data;

    if (inter->current_data == NULL) {
        inter->current_data = (TAData *) TA_SAFE_MALLOC(sizeof(TAData));
        if (inter->current_data == NULL) {
            return -1;
        }
        memset(inter->current_data, 0, sizeof(TAData));
    }

    current_data = inter->current_data;
    data = (char *) TA_SAFE_REALLOC(current_data->data, current_data->body_size + length + 2);
    if (data == NULL) {
        return -1;
    }
    current_data->data = data;

    memcpy(current_data->data + current_data->body_size, event, length);
    current_data->body_size += length;
    current_data->data[current_data->body_size] = '\0';
    current_data->size++;
    return current_data->size;
}

static int ta_batch_consumer_add(void *this_, const char *event, unsigned long length) {
    TABatchConsumerInter *inter;
    int count;

    if (NULL == this_ || NULL == event) {
        return TA_INVALID_PARAMETER_ERROR;
    }

    inter = (TABatchConsumerInter *) this_;
    count = add_to_list(inter, event, length);
    if (count == -1) {
        return TA_MALLOC_ERROR;
    }

    ta_batch_consumer_flush(inter);

    return TA_OK;
}

static void get_config_param_of_batch(TABatchConsumerInter *inter, const TAConfig *config) {
    TAListNode *curr = config->value.child;
    while (NULL != curr) {
        if (NULL != curr->value->key) {
            if (0 == strncmp(TA_CONFIG_DEBUG_MODE, curr->value->key, 256)) {
                TANode *debug_model = curr->value;
                if (NULL != debug_model) {
                    inter->debug_mode = (int) debug_model->value.int_;
                }
            } else if (0 == strncmp(TA_CONFIG_PUSH_URL, curr->value->key, 256)) {
                TANode *push_url_node = curr->value;
                if (NULL != push_url_node) {
                    snprintf(inter->push_url, 1024, "%s", push_url_node->value.string_);
                    if (inter->push_url[strlen(inter->push_url) - 1] == '/') {
                        strcat(inter->push_url, "data_debug");
                    } else {
                        strcat(inter->push_url, "/data_debug");
                    }
                }
            } else if (0 == strncmp(TA_CONFIG_APPID, curr->value->key, 256)) {
                TANode *appid_node = curr->value;
                if (NULL != appid_node) {
                    snprintf(inter->appid, 50, "%s", appid_node->value.string_);
                }
            } else if (0 == strncmp(TA_CONFIG_TIMEOUT, curr->value->key, 256)) {
                TANode *timeout_node = curr->value;
                if (NULL != timeout_node) {
                    inter->timeout = (int) timeout_node->value.int_;
                }
            } else if (0 == strncmp(TA_CONFIG_LOG, curr->value->key, 256)) {
                TANode *log_node = curr->value;
                if (NULL != log_node) {
                    inter->log = log_node->value.boolean_;
                }
            }
        }
        curr = curr->next;
    }
}

int ta_init_consumer(struct TAConsumer **ta, const TAConfig *config) {
    struct TAConsumer *consumer;
    TABatchConsumerInter *inter = (TABatchConsumerInter *) TA_SAFE_MALLOC(sizeof(TABatchConsumerInter));
    if (inter == NULL) {
        return TA_MALLOC_ERROR;
    }
    memset(inter, 0, sizeof(TABatchConsumerInter));

    inter->batch_size = 1;
    inter->max_cache_size = 1;
    inter->timeout = 30;
    inter->debug_mode = 0;

    if (config != NULL) {
        get_config_param_of_batch(inter, config);
    }

    consumer = (struct TAConsumer *) TA_SAFE_MALLOC(sizeof(struct TAConsumer));
    if (consumer == NULL) {
        return TA_MALLOC_ERROR;
    }
    memset(consumer, 0, sizeof(struct TAConsumer));

    consumer->this_ = (void *) inter;
    consumer->op.add = &ta_batch_consumer_add;
    consumer->op.flush = &ta_batch_consumer_flush;
    consumer->op.close = &ta_batch_consumer_close;

    *ta = consumer;
    return TA_OK;
}
