#ifndef REQUEST_H
#define REQUEST_H

#include <stdlib.h>

typedef struct 
{
    char *method;
    char *url;
    char *version;

    struct 
    {
        char *name;
        char *value;
    } *headers;
    size_t headers_n;

    char *body;
} Request;



Request parse_request(char *data, ssize_t length);

#endif