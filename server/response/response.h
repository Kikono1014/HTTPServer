#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdlib.h>
#include "../consts.h"

typedef struct 
{
    int code;
    char *status;
    char *headers;
    u_int8_t *body;
    size_t body_size;
} Response;



size_t build_response(u_int8_t buffer[BUFFER_SIZE], Response response);
void write_string(Response *response, char* string);

#endif