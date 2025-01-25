#include "response.h"
#include <stdio.h>
#include <string.h>
#include "../consts.h"

size_t build_response(u_int8_t buffer[BUFFER_SIZE], Response response)
{
    memset(buffer, 0, BUFFER_SIZE);
    sprintf(buffer, 
        "HTTP/1.1 %d %s\r\n%s\r\n",
        response.code, response.status,
        response.headers
    );
    size_t size = strlen(buffer);

    memcpy(buffer + size, response.body, response.body_size);
    size += response.body_size;
    
    return size;
}

void write_string(Response *response, char *string)
{
    response->body_size = strlen(string);
    response->body = malloc(response->body_size);
    strcpy(response->body, string);
}