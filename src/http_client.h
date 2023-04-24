
#ifndef TA_C_SDK_HTTP_CLIENT_H
#define TA_C_SDK_HTTP_CLIENT_H

typedef struct {
    long status;
    char *body;
    unsigned long body_size;
} HttpResponse;

HttpResponse *ta_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds);

HttpResponse *ta_debug_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds, int dryRun, const char* device_id);

void destroy_http_response(HttpResponse *response);

#endif
