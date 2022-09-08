#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "../src/thread-pool.h"
#include "../src/http-api.h"
#include "../src/http-request.h"
#include "../src/http-channel.h"

// char contentType[] = "text/html; charset=UTF-8";
char contentType[] = "text/html";
char body[] = "<html><head></head><body><div>Hello</div></body></html>";
char server[] = "Server";
char serverId[] = "Epoll srv/1.0.0";

void testHttpHandlerFunc(HttpRequest* req, HttpResponse* resp) {
    // printf("> req path: %s %s\n", req->method, req->path);

    // for(int i = 0; i < req->headerIndex; i++ ) {
    //     printf("> req header: %s: %s\n", req->headers[i]->key, req->headers[i]->value);
    // }
    // if (req->body != NULL) {
    //     printf("> req body: %s\n", req->body);
    // }

    resp->contentType = malloc(sizeof(char) * strlen(contentType) + 1);
    strcpy(resp->contentType, contentType);

    // resp->body = NULL;
    resp->body = malloc(sizeof(char) * strlen(body) + 1);
    strcpy(resp->body, body);

    resp->headers = (HttpHeader**)malloc(1 * sizeof(HttpHeader*));
    
    resp->headers[0] = (HttpHeader*) malloc(sizeof(HttpHeader));
    
    resp->headers[0]->key = malloc(sizeof(char) * strlen(server) + 1);
    strcpy(resp->headers[0]->key, server);
    
    resp->headers[0]->value = malloc(sizeof(char) * strlen(serverId) + 1);
    strcpy(resp->headers[0]->value, serverId);
    
    resp->headerIndex = 1;
    resp->status = NULL;
    // resp->status = "HTTP/1.1 200 OK\n";
    // usleep(50000);
}

void testHttpApi() {
    ThreadPoolState* state = threadPool_start(1, 20);

    InitParams initProps;
    initProps.bindIpAddress = "0.0.0.0";
    initProps.bindPort = 8000;
    initProps.maxConnections = 50000;
    initProps.func = &http_channel_task_readSocket;

    HttpApiProperties* props = httpapi_start(&initProps, state);

    HttpHandler* handler = (HttpHandler*) malloc(sizeof(HttpHandler));
    handler->path = "/";
    handler->func = &testHttpHandlerFunc;
    httpapi_addHandler(props, handler);

    HttpHandler* handler2 = (HttpHandler*) malloc(sizeof(HttpHandler));
    handler2->path = "/audio";
    handler2->func = &testHttpHandlerFunc;
    httpapi_addHandler(props, handler2);

    pthread_exit(NULL);

    // httpapi_stop(props);
    // threadPool_stop(state);
}


int main(void* args) {
    // signal(SIGPIPE, SIG_IGN);

    testHttpApi();
}