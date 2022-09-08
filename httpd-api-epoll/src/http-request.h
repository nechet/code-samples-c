#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#define ROW_DELIMITER "\r\n"
#define HEADER_DELIMITER ":"

typedef struct HttpHeader {
    char* key;
    char* value;
} HttpHeader;

typedef struct HttpRequest {
    HttpHeader** headers;
    int   headerIndex;
    char* method;
    char* path;
    char* protocol;
    char* body;
    char* headerContentType;
    int   headerContentLength;
    char* headerHost;
    char* headerConnection;
} HttpRequest;


void split(char* input, char** output, char* delimiter);
void httpRequest_parseHeaders(char* input, HttpRequest* httpRequest);
void httpRequest_parseRequest(char* input, HttpRequest* httpRequest);

#endif
