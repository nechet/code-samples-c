#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "http-request.h"

void split(char* input, char** output, char* delimiter) {
    char* token = strtok(input, delimiter);

    for(int i = 0; token != NULL; i++) {
        output[i] = token;
        token = strtok(NULL, delimiter);
    }
}

void httpRequest_parseMethod(char* pathRow, HttpRequest* httpRequest, char* delimiter ) {
    char** output = (char**) malloc(5 * sizeof(char*));
    
    split (pathRow, output, delimiter);
    
    // httpRequest->method = output[0];
    int lengthMethod = sizeof(char) * strlen(output[0]);
    httpRequest->method = malloc (lengthMethod + 1);
    strcpy (httpRequest->method, output[0]);
    httpRequest->method[lengthMethod] = 0;

    // httpRequest->path = output[1];
    int lengthPath = sizeof(char) * strlen(output[1]);
    httpRequest->path = malloc (lengthPath + 1);
    strcpy (httpRequest->path, output[1]);
    httpRequest->path[lengthPath] = 0;

    int lengthProtocol = sizeof(char) * strlen(output[2]);
    httpRequest->protocol = malloc (lengthProtocol + 1);
    strcpy (httpRequest->protocol, output[2]);
    httpRequest->protocol[lengthProtocol] = 0;

    free(output);
}

void httpRequest_parseHeader(char* input, HttpHeader* header ) {
        char* token = strtok(input, HEADER_DELIMITER);
        
        // header->key = token;
        int lengthKey = sizeof(char) * strlen(token);
        header->key = malloc (lengthKey + 1);
        strcpy (header->key, token);
        header->key[lengthKey] = 0;
        
        // header->value = (input + strlen(token) + 2);
        int lengthValue = sizeof(char) * strlen(input + strlen(token) + 2);
        header->value = malloc (lengthValue + 1);
        strcpy (header->value, input + strlen(token) + 2);
        header->value[lengthValue] = 0;
}

void httpRequest_parseHeaderValues(HttpHeader* header, HttpRequest* httpRequest) {
    int lengthValue = sizeof(char) * strlen(header->value);

    if (strcmp("Host", header->key) == 0) {
        // httpRequest->headerHost = header->value;
        httpRequest->headerHost = malloc(lengthValue + 1);
        strcpy(httpRequest->headerHost, header->value);
        httpRequest->headerHost[lengthValue] = 0;

    } else if (strcmp("Content-Type", header->key) == 0) {
        // httpRequest->headerContentType = header->value;
        httpRequest->headerContentType = malloc (lengthValue + 1);
        strcpy(httpRequest->headerContentType, header->value);
        httpRequest->headerContentType[lengthValue] = 0;

    } else if (strcmp("Content-Length", header->key) == 0) {
        httpRequest->headerContentLength = atoi(header->value);

    } else if (strcmp("Connection", header->key) == 0) {
        // httpRequest->headerConnection = header->value;
        httpRequest->headerConnection = malloc (lengthValue + 1);
        strcpy(httpRequest->headerConnection, header->value);
        httpRequest->headerConnection[lengthValue] = 0;
    }
}

void httpRequest_parseHeaders(char* input, HttpRequest* httpRequest) {
    char** output = (char**) malloc(20 * sizeof(char*));
    char* token = strtok(input, ROW_DELIMITER);
    int i;
    for (i = 0; token != NULL; i++) {
        output[i] = token;
        token = strtok(NULL, ROW_DELIMITER);
    }

    httpRequest_parseMethod(output[0], httpRequest, " ");

    httpRequest->headerIndex = 0;
    httpRequest->headers = malloc(i * sizeof(HttpHeader*));

    for (int j = 1; j < i; j++) {
        HttpHeader* header = malloc(sizeof(HttpHeader));
        httpRequest_parseHeader(output[j], header);
        httpRequest->headers[httpRequest->headerIndex] = header;
        httpRequest->headerIndex++;

        httpRequest_parseHeaderValues(header, httpRequest);
    }

    free(output);
}


void httpRequest_parseRequest(char* input, HttpRequest* httpRequest) {
    char* bodyPtr = strstr(input, "\r\n\r\n");
    if (bodyPtr != NULL) {
        int bodyLength = strlen(bodyPtr);
        // printf(">>>> body: %s:%d\n", bodyPtr, bodyLength);
        bodyPtr[0] = 0;
        if (bodyLength > 4) {
            // httpRequest->body = (bodyPtr + strlen("\r\n\r\n"));
            int lengthBody = strlen(bodyPtr + strlen("\r\n\r\n")) * sizeof(char);
            httpRequest->body = malloc(lengthBody + 1);
            strcpy(httpRequest->body, (bodyPtr + strlen("\r\n\r\n")));
            httpRequest->body[lengthBody] = 0;
        }
    }
    httpRequest_parseHeaders(input, httpRequest);
}
