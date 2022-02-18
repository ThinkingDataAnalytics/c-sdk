#include "thinkingdata_private.h"
#include "util.h"
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

const char TA_CONFIG_ROTATE_MODE[] = "rotate_mode";
const char TA_CONFIG_FILE_SIZE[] = "file_size";
const char TA_CONFIG_FILE_PATH[] = "file_path";
const char TA_CONFIG_FILE_PREFIX[] = "file_prefix";

typedef struct {
    int file_size;
    TABool log;
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
    TALoggingConsumerInter *inter;

    if (NULL == this_) {
        return TA_INVALID_PARAMETER_ERROR;
    }

    inter = (TALoggingConsumerInter *) this_;
    if (NULL != inter->file) {
        fflush(inter->file);
        fclose(inter->file);
        inter->file = NULL;
        inter->last_file_count = 0;
    }

    return TA_OK;
}

static TABool file_size(char *file_name, int file_size) {
    struct stat buffer;
    long size;

    stat(file_name, &buffer);
    size = buffer.st_size / (1024 * 1024);
    if (size >= file_size) {
        return TA_TRUE;
    } else {
        return TA_FALSE;
    }
}

static int ta_logging_consumer_add(void *this_, const char *event, unsigned long length) {
    TALoggingConsumerInter *inter;
    struct tm tm;
    time_t t;
    char *file_prefix = NULL, *file_name_date = NULL;
    size_t prefixLength;
    int count = 0;
    TABool need_new_file = TA_FALSE;

    if (NULL == this_ || NULL == event) {
        return TA_INVALID_PARAMETER_ERROR;
    }

    inter = (TALoggingConsumerInter *) this_;
    t = time(NULL);
    LOCALTIME(&t, &tm);
    prefixLength = strlen(inter->file_prefix);
    if (prefixLength > 0) {
        file_prefix = (char *) malloc(prefixLength + 4 + 1);
        strcpy(file_prefix, inter->file_prefix);
        strcat(file_prefix, ".log");
    } else {
        file_prefix = (char *) malloc(3 + 1);
        strcpy(file_prefix, "log");
    }

    file_name_date = TA_SAFE_MALLOC(512);
    if (file_name_date == NULL) {
        return TA_MALLOC_ERROR;
    }
    if (inter->rotate_mode == DAILY) {
        snprintf(file_name_date, 512, "%s.%4d-%02d-%02d_", file_prefix, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    } else {
        snprintf(file_name_date, 512, "%s.%4d-%02d-%02d-%02d_", file_prefix, tm.tm_year + 1900, tm.tm_mon + 1,
                 tm.tm_mday, tm.tm_hour);
    }
    TA_SAFE_FREE(file_prefix);

    if (strcmp(inter->last_file_date, file_name_date) == 0) {
        if (inter->file_size > 0) {
            long size;
            count = inter->last_file_count;
            fseek(inter->file, 0, SEEK_END);
            size = ftell(inter->file) / (1024 * 1024);
            if (size >= inter->file_size) {
                count++;
                need_new_file = TA_TRUE;
            }
        } else {
            need_new_file = TA_FALSE;
        }
    } else {
        need_new_file = TA_TRUE;
        snprintf(inter->last_file_date, 512, "%s", file_name_date);
    }

    if (need_new_file == TA_TRUE) {
        FILE *file = NULL;
        unsigned int times = 0;
        char *file_path = TA_SAFE_MALLOC(1024);
        if (file_path == NULL) {
            return TA_MALLOC_ERROR;
        }
        ta_logging_consumer_close(this_);
        snprintf(file_path, 1024, "%s/%s%d", inter->file_path, file_name_date, count);

        if (inter->file_size > 0) {
            while (file_exists(file_path) == 0 && file_size(file_path, inter->file_size)) {
                count++;
                snprintf(file_path, 1024, "%s/%s%d", inter->file_path, file_name_date, count);
            }
        }

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
    fflush(inter->file);

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
                    inter->log = log_node->value.boolean_;
                }
            }
        }
        curr = curr->next;
    }
}

int ta_init_consumer(struct TAConsumer **ta, const TAConfig *config) {
    struct TAConsumer *ta_consumer;
    TALoggingConsumerInter *inter = (TALoggingConsumerInter *) TA_SAFE_MALLOC(sizeof(TALoggingConsumerInter));
    if (inter == NULL) {
        return TA_MALLOC_ERROR;
    }
    memset(inter, 0, sizeof(TALoggingConsumerInter));

    inter->last_file_count = 0;
    if (config != NULL) {
        get_config_param(inter, config);
    }

    ta_consumer = (struct TAConsumer *) TA_SAFE_MALLOC(sizeof(struct TAConsumer));
    if (ta_consumer == NULL) {
        return TA_MALLOC_ERROR;
    }
    memset(ta_consumer, 0, sizeof(struct TAConsumer));

    ta_consumer->this_ = (void *) inter;
    ta_consumer->op.add = &ta_logging_consumer_add;
    ta_consumer->op.flush = &ta_logging_consumer_flush;
    ta_consumer->op.close = &ta_logging_consumer_close;

    *ta = ta_consumer;

    return TA_OK;
}

