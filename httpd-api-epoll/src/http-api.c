#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "http-api.h"
#include "http-request.h"
#include "http-channel.h"


void msg(const char* txt) {
    printf(">> %s\n", txt);
}

int httpapi_create_srv_socket(InitParams* params) {
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(params->bindIpAddress);
    serv_addr.sin_port = htons(params->bindPort);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Can't bind to port\n");
        exit(1);
    }
    if (listen(serv_sock, params->maxConnections) < 0) {
        perror("Can't open listen socket\n");
        exit(1);
    }
    return serv_sock;
}


void* httpapi_accept_incoming_connections(void *args) {
    HttpApiProperties* props = (HttpApiProperties*) args;

    int sock = props->sock;
    int epollfd = props->epollfd;
    int maxBuffers = 50; // props->maxConnections;

    int requestsProcessed = 0;

    struct epoll_event event_buffers[maxBuffers];

    int bufferSizeInBytes = 4096 * sizeof(char);
    // char* buffer = malloc(bufferSizeInBytes + 1);

    while (1) {
        int n = epoll_wait (epollfd, event_buffers, maxBuffers, -1);
        if (n < 0) {
            perror("Problem with epoll_wait call");
            continue;
        }
        printf("Events: %d\n", n);

        for (int i = 0; i < n; i++) {

            if (event_buffers[i].data.fd == sock) { // incoming request
                // struct sockaddr sock_addr;
                struct sockaddr_in srv_addr;
                socklen_t srv_length = sizeof(srv_addr);

                int conn_sock = accept(sock, (struct sockaddr *)&srv_addr, &srv_length);
                if (conn_sock < 0) {
                    if (EAGAIN == errno || EWOULDBLOCK == errno) {
                        perror("EAGAIN on Accept connection");
                    } else {
                        perror("Failed to Accept connection");
                    }
                    continue;
                }

                if (fcntl(conn_sock, F_SETFL, fcntl(conn_sock, F_GETFL, 0) | O_NONBLOCK) < 0) {  /* non-blocking */
                    perror("Failed to change socket to Non-Blocked");
                }

                struct epoll_event epollEvent;
                // epollEvent.events = EPOLLIN | EPOLLET | EPOLLONESHOT; /* incoming, edge-triggered */
                epollEvent.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                epollEvent.data.fd = conn_sock;

                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &epollEvent) < 0) {
                    perror("Problem with epoll_ctl register connection");
                }
            } else {
                Channel* channel = malloc(sizeof(Channel));
                channel->props = props;
                // channel->req = NULL;
                // channel->resp = NULL;
                channel->sock = event_buffers[i].data.fd;

                Task* task = (Task*) malloc(sizeof(Task));
                task->task = props->func;
                task->taskArg1 = channel;
                task->taskArg2 = NULL;
                task->taskArg3 = NULL;

                threadPool_pushTask(props->threadPoolState, task);
            }
        }
    }

    // free(buffer);
    return 0;
}


/***** Start *****/
HttpApiProperties* httpapi_start (InitParams* params, ThreadPoolState* threadPoolState) {
    // signal(SIGPIPE, SIG_IGN);
    msg ("HTTP API starting...");

    int epollfd = epoll_create(params->maxConnections + 1);
    if (epollfd < 0) {
        perror("Problem with epoll_create\n");
        exit(1);
    }

    int sock = httpapi_create_srv_socket(params);

    /* polling */
    struct epoll_event epollEvent;
    epollEvent.events = EPOLLIN; // | EPOLLET; /* incoming, edge-triggered */
    epollEvent.data.fd = sock;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &epollEvent) < 0) {
        perror("Problem with epoll_ctl call");
        exit(1);
    }

    HttpApiProperties* props = (HttpApiProperties *) malloc(sizeof(HttpApiProperties));
                        props->maxConnections = params->maxConnections;
                        props->sock = sock;
                        props->epollfd = epollfd;
                        props->httpHandlerIndex = 0;
                        props->httpHandlers = malloc(10 * sizeof(HttpHandler*));
                        props->threadPoolState = threadPoolState;
                        props->func = params->func;

    int err = pthread_create(&props->threadId, NULL, &httpapi_accept_incoming_connections, props);

    msg ("HTTP API started...");

    return props;
}


void httpapi_stop (HttpApiProperties* httpdPropeerties) {
    pthread_cancel (httpdPropeerties->threadId);
    // int err_join = pthread_join(threadId, NULL);
    // if (err_join)
    // {
    //     return err_join;
    // }

    close (httpdPropeerties->epollfd);

    free(httpdPropeerties->httpHandlers);
    free(httpdPropeerties);

    msg ("HTTP API stopped...");
}


void httpapi_addHandler (HttpApiProperties* props, HttpHandler* httpHandler) {
    props->httpHandlers[props->httpHandlerIndex++] = httpHandler;
}

HttpHandler* httpapi_getHandler (HttpApiProperties* props, HttpRequest* httpRequest) {
    HttpHandler* handler = NULL;
    char* reqParams = strchr(httpRequest->path, '?');

    for (int i = 0; i < props->httpHandlerIndex; i++) {
        char* path = props->httpHandlers[i]->path;
        if (reqParams == NULL && strcmp(path, httpRequest->path) == 0) {
            handler = props->httpHandlers[i];
            break;
        } else if (reqParams != NULL) {
            int reqPathLegth = reqParams - httpRequest->path;
            if (reqPathLegth == strlen(path) && strncmp(path, httpRequest->path, reqPathLegth) == 0) {
                handler = props->httpHandlers[i];
                break;
            }
        }
    }
    // printf(">> handler: %s, request: %s\n", handler == NULL ? "Not Found" : handler->path, httpRequest->path);
    return handler;
}
