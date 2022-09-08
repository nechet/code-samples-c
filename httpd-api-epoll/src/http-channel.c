#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "http-channel.h"
#include "thread-pool.h"


void http_channel_task_exec (Channel* channel, HttpRequest* req, HttpResponse* resp) {

    HttpResponse* httpResponse = (HttpResponse*) malloc(sizeof(HttpResponse));
    // memset(httpResponse, 0, sizeof(HttpResponse));
    httpResponse->headerIndex = 0;
    httpResponse->body = NULL;
    httpResponse->contentType = NULL;
    httpResponse->status = NULL;

    HttpHandler* handler = httpapi_getHandler(channel->props, req);

    char* output = NULL;

    if (handler == NULL) {
        int length = httpResponse_sendResponse_404_length(httpResponse);
        output = (char*) malloc(length * sizeof(char) + 1);
        memset(output, 0, sizeof(char) * length + 1);
        httpResponse_sendResponse_404(httpResponse, output);
    } else {
        handler->func(req, httpResponse);

        // httpResponse_sendResponse_404(httpResponse);

        int length = httpResponse_sendResponse_200_length(httpResponse);
        output = (char*) malloc(length * sizeof(char) + 1);
        memset(output, 0, sizeof(char) * length);
        httpResponse_sendResponse_200(httpResponse, output);

        // int length = httpResponse_sendResponse_404_length(httpResponse);
        // output = (char*) malloc(length * sizeof(char) + 1);
        // memset(output, 0, sizeof(char) * length);
        // httpResponse_sendResponse_404(httpResponse, output);

    }
    ssize_t sent = send(channel->sock, output, strlen(output) * sizeof(char), 0);
    if (sent == -1) {
        perror("Failed to send response");
    }

    if (req->headerConnection == NULL || strcmp(req->headerConnection, "close") == 0) {
        if (epoll_ctl (channel->props->epollfd, EPOLL_CTL_DEL, channel->sock, NULL) < 0) {
            perror("Problem with epoll_ctl unregister connection");
        }
        close(channel->sock);
        // printf("Connection closed...\n");
    }


    for (int i = 0; i < req->headerIndex; i++) {
        free(req->headers[i]->key);
        free(req->headers[i]->value);
        free(req->headers[i]);
    }
    free(req->headers);
    if (req->headerConnection != NULL) {
        free(req->headerConnection);
    }
    if (req->headerContentType != NULL) {
        free(req->headerContentType);
    }
    if (req->method != NULL) {
        free(req->method);
    }
    if (req->path != NULL) {
        free(req->path);
    }
    if (req->protocol != NULL) {
        free(req->protocol);
    }
    if (req->headerHost != NULL) {
        free(req->headerHost);
    }
    if (req->body != NULL) {
        free(req->body);
    }
    free(req);

    // resp
    if (httpResponse->body != NULL) {
        free(httpResponse->body);
    }
    free(httpResponse->contentType);

    for (int i = 0; i < httpResponse->headerIndex; i++) {
        free(httpResponse->headers[i]->key);
        free(httpResponse->headers[i]->value);
        free(httpResponse->headers[i]);
    }
    free(httpResponse->headers);
    free(httpResponse);

    free(output);
}


void http_channel_task_exec_wrapper(void* arg1, void* arg2, void* arg3) {
    http_channel_task_exec(arg1, arg2, arg3);
}


void http_channel_task_readSocket (Channel* channel, HttpRequest* req, HttpResponse* resp) {
    int sock_read = channel->sock;

    int bufferSizeInBytes = 4096 * sizeof(char);
    char* buffer = malloc (bufferSizeInBytes + 1);
    memset(buffer, 0, bufferSizeInBytes);

    int bytes_read = recv(sock_read, buffer, bufferSizeInBytes, MSG_DONTWAIT);
    if (bytes_read < 0) {
        free(buffer);
        perror("Failed to read socket");

        return;
    }
    // printf("read bytes %d from socket %d\n", bytes_read, sock_read);
    if (bytes_read == 0) {
        free(buffer);
        printf("read bytes %d from socket %d\n", bytes_read, sock_read);

        return;
    }
    buffer[bytes_read] = 0;
    // printf(">> %s\n", buffer);

    HttpRequest* httpRequest = malloc(sizeof(HttpRequest));
    memset(httpRequest, 0, sizeof(HttpRequest));
    httpRequest->body = NULL;
    httpRequest->method = NULL;
    httpRequest->path = NULL;
    httpRequest->headerHost = NULL;
    httpRequest->headerConnection = NULL;
    httpRequest->headerContentType = NULL;
    httpRequest->headers = NULL;
    httpRequest->protocol = NULL;

    httpRequest_parseRequest(buffer, httpRequest);
    free(buffer);

    http_channel_task_exec(channel, httpRequest, resp);

    // Task* task = (Task*) malloc(sizeof(Task));
    // task->task = &http_channel_task_exec_wrapper;
    // task->taskArg1 = channel;
    // task->taskArg2 = httpRequest;
    // task->taskArg3 = resp;

    // threadPool_pushTask(channel->props->threadPoolState, task);
}

