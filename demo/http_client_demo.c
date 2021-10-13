//
// Created by lizhiming on 2021/10/11.
//

#include <string.h>
#include <stdio.h>

#include "../src/http_client.h"
#include <thinkingdata.h>

static HttpResponse *ta_http_post_json(const char *appid, const char *url, const char *data) {
    return ta_http_post(appid, url, data,1, strlen(data),30);
}

int main() {
    HttpResponse *response = ta_http_post_json("", "https://ta-dev.thinkingdata.cn", "{\"key\": 787}");
    TA_ASSERT(response != NULL);

    printf("response: status=%ld, body=%s\n", response->status, response->body);

    destroy_http_response(response);

    return 0;
}
