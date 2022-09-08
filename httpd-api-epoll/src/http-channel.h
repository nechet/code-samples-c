#ifndef HTTP_CHANNEL_H
#define HTTP_CHANNEL_H

#include "http-request.h"
#include "http-response.h"
#include "http-api.h"


typedef struct Channel {
    HttpApiProperties* props;
                   int sock;
        //   HttpRequest* req;
        //  HttpResponse* resp;
} Channel;

void http_channel_task_exec (Channel* channel, HttpRequest* req, HttpResponse* resp);
void http_channel_task_readSocket (Channel* channel, HttpRequest* req, HttpResponse* resp);

#endif