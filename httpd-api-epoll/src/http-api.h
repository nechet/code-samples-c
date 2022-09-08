#ifndef HTTP_API_H
#define HTTP_API_H

#include <pthread.h>
#include "http-request.h"
#include "http-response.h"
#include "thread-pool.h"

#define true 1
#define false 0

// ----- init -----
typedef struct InitParams {
    char *bindIpAddress;
    int bindPort;
    int maxConnections;
    void (*func)(void* arg1, void* arg2, void* arg3);
} InitParams;

// ---- add handlers -----
// typedef void (*HttpHandlerFunc)(HttpRequest*, HttpResponse*);

typedef struct HttpHandler {
    char* path;
    // HttpHandlerFunc handlerFunc;
    void (*func)(HttpRequest* req, HttpResponse* resp);
} HttpHandler;

typedef struct HttpApiProperties {
                int maxConnections;
                int sock;
                int epollfd;
          pthread_t threadId;
      HttpHandler** httpHandlers;
                int httpHandlerIndex;
   ThreadPoolState* threadPoolState;
        void (*func)(void* arg1, void* arg2, void* arg3);
} HttpApiProperties;


HttpApiProperties* httpapi_start (InitParams* params, ThreadPoolState* threadPoolState);
              void httpapi_stop (HttpApiProperties* httpApiProperties);
              void httpapi_addHandler (HttpApiProperties* props, HttpHandler* httpHandler);
      HttpHandler* httpapi_getHandler (HttpApiProperties* props, HttpRequest* httpRequest);

#endif
