#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "http-request.h"

typedef struct HttpResponse {
    HttpHeader** headers;
    int   headerIndex;
    char* body;
    char* status;
    char* contentType;
} HttpResponse;

int  httpResponse_sendResponse_404_length(HttpResponse* resp);
void httpResponse_sendResponse_404(HttpResponse* resp, char* output);

int  httpResponse_sendResponse_200_length(HttpResponse* resp);
void httpResponse_sendResponse_200(HttpResponse* resp, char* output);

#endif
