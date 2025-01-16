#include "request.h"
#include <stdio.h>
#include <string.h>


Request parse_request(char *data, ssize_t length)
{
    size_t i = 0;

    Request request = {
        .headers = calloc(1, sizeof(*request.headers))
    };

    char *outer_saveptr = NULL;
    char *inner_saveptr = NULL;
    char *inner_saveptr1 = NULL;
    char *token = strtok_r(data, "\n", &outer_saveptr);

    i += strlen(token);

    char *line_token = strtok_r(token, " ", &inner_saveptr1);
    request.method = line_token;

    line_token = strtok_r(NULL, " ", &inner_saveptr1);
    request.url = line_token;

    line_token = strtok_r(NULL, " ", &inner_saveptr1);
    request.version = line_token;


    
    token = strtok_r(NULL, "\n", &outer_saveptr);
    while (token != NULL)
    {
        i += strlen(token) + 1;

        if (strlen(token) == 1)
        {
            break;
        }

        char *name = strtok_r(token, ":", &inner_saveptr);
        char *value = strtok_r(NULL, ":", &inner_saveptr);
        memcpy(value, value+1, strlen(value)-1);

        request.headers_n++;
        request.headers = reallocarray((void*)request.headers, request.headers_n, sizeof(*request.headers));
        request.headers[request.headers_n-1].name = name;
        request.headers[request.headers_n-1].value = value;

        token = strtok_r(NULL, "\n", &outer_saveptr);
    }
    
    request.body = malloc(length - i);
    strncpy(request.body, data + i, length - i);
    
    
    return request;
}
