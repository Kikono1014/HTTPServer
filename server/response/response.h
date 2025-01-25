#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdlib.h>

typedef struct 
{
    int code;
    char *status;
    char *headers;
    char *body;
} Response;



char *build_response(Response response);

#endif