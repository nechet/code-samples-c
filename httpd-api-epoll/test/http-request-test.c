#include <stdlib.h>
#include <stdio.h>

#include "../src/http-request.h"

void main (char* args) {

    char request[1024] = "GET / HTTP/1.1\nHost: localhost:8000\nUser-Agent: curl/7.68.0\nAccept: */*\n";

    printf("%s\n", request);

    HttpRequest* req = (HttpRequest*) malloc(sizeof(HttpRequest));

    httpRequest_parseHeaders(request, req);

    printf("%s %s\n", req->method, req->path);

    for(int i = 0; i < req->headerIndex; i++) {
        HttpHeader* header = req->headers[i];
        printf("%s: %s\n", header->key, header->value);
    }

}