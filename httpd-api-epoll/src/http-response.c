#include "http-response.h"

#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "time-utils.h"

#define DELIMITER_ROW "\r\n"

char status_200[] = "HTTP/1.1 200 OK\r\n";
char status_404[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: 0\r\n";
// char status_404[] = "HTTP/1.1 404 Not Found\r\n";
// char status_404[] = "HTTP/1.1 200 OK\r\nServer: Epoll srv/1.0.0\r\nContent-Type: text/html\r\nContent-Length: 7\r\n\r\nHello\r\n";


int httpResponse_sendResponse_404_length(HttpResponse* resp) {
    return strlen(status_404);
}

void httpResponse_sendResponse_404(HttpResponse* resp, char* output) {
    // send(resp->requestSock, status_404, strlen(status_404), 0);
    strcpy(output, status_404);
}

int httpResponse_sendResponse_200_length(HttpResponse* resp) {
    char* bodyLength = (char*) malloc(10 * sizeof(char));

    int lengthResponse = 0;
    lengthResponse += strlen(status_200);
    for(int j = 0; j < resp->headerIndex; j++) {
        HttpHeader* header = resp->headers[j];
        lengthResponse += (strlen(header->key) + strlen(": ") + strlen(header->value) + strlen(DELIMITER_ROW));
    }
    lengthResponse += (strlen("Content-Type: ") + strlen(resp->contentType) + strlen(DELIMITER_ROW));
    sprintf(bodyLength, "%ld", resp->body == NULL ? 0 : strlen(resp->body) + strlen(DELIMITER_ROW) );
    lengthResponse += (strlen("Content-Length: ") + strlen(bodyLength) + strlen(DELIMITER_ROW));
    
    char* time = malloc(sizeof(char) * 50);
    timestamp(time);

    lengthResponse += (strlen("Date: ") + strlen(time) + strlen(DELIMITER_ROW));
    free(time);

    if (resp->body != NULL) {
        lengthResponse += strlen(DELIMITER_ROW);
        lengthResponse += strlen(resp->body);
        lengthResponse += strlen(DELIMITER_ROW);
    }

    free(bodyLength);
    return lengthResponse;
}

void httpResponse_sendResponse_200(HttpResponse* resp, char* output) {
    char* bodyLength = (char*) malloc(10 * sizeof(char));

    strcat(output, status_200);

    for(int j = 0; j < resp->headerIndex; j++) {
        HttpHeader* header = resp->headers[j];
        strcat(output, header->key);
        strcat(output, ": ");
        strcat(output, header->value);
        strcat(output, DELIMITER_ROW);
    }
    
    char* time = malloc(sizeof(char) * 50);
    timestamp(time);

    strcat(output, "Date: ");
    strcat(output, time);
    strcat(output, DELIMITER_ROW);
    free(time);

    strcat(output, "Content-Type: ");
    strcat(output, resp->contentType);
    strcat(output, DELIMITER_ROW);

    sprintf(bodyLength, "%ld", resp->body == NULL ? 0 : strlen(resp->body) + strlen(DELIMITER_ROW) );
    strcat(output, "Content-Length: ");
    strcat(output, bodyLength);
    strcat(output, DELIMITER_ROW);

    if (resp->body != NULL) {
        strcat(output, DELIMITER_ROW);
        strcat(output, resp->body);
        strcat(output, DELIMITER_ROW);
    }
    free(bodyLength);
}
