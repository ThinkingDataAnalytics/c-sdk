//
// Created by lizhiming on 2021/10/11.
//

#ifndef TA_C_SDK_HTTP_CLIENT_H
#define TA_C_SDK_HTTP_CLIENT_H

typedef struct {
    long status;
    char *body;
    unsigned long body_size;
} HttpResponse;

HttpResponse *ta_http_post(
        const char *appid, const char *url, const char *data, int data_size, size_t data_length, int timeout_seconds);

void destroy_http_response(HttpResponse *response);

#endif //TA_C_SDK_HTTP_CLIENT_H
