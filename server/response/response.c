#include "response.h"
#include <stdio.h>
#include <string.h>


char *build_response(Response response)
{

    char *data = malloc(8 + 1 + 3 + 1 + strlen(response.status)
        + 1 + strlen(response.headers)
        + 1
        + 2 + strlen(response.body)
    );
    
    sprintf(data, 
        "HTTP/1.1 %d %s\n%s\n \n%s",
        response.code, response.status,
        response.headers, response.body
    );

    return data;
}
