#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdlib.h>
#include "../request/request.h"

typedef struct 
{
    int code;
    char *status;
    char *headers;
    u_int8_t *body;
    size_t body_size;
} Response;



Response build_response(Request request);
size_t compile_response(u_int8_t *buffer, Response response);
void respond_string(Response *response, char* string);
void respond_file(Response *response, char* path);

#endif