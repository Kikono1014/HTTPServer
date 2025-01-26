#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>
#include <stdint.h>


#include "../consts.h"
#include "../request/request.h"

void respond_string(Response *response, char *string)
{
    response->body_size = strlen(string);
    response->body = malloc(response->body_size);
    strcpy(response->body, string);
}


size_t read_file(char *path, uint8_t **buffer)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    *buffer = malloc(fsize);
    fread(*buffer, fsize, 1, f);
    fclose(f);

    return fsize;
}


char *get_content_type_header(char *extension)
{
    if      (extension == "html") return "Content-type: text/html";
    else if (extension == "css")  return "Content-type: text/css";
    else if (extension == "js")   return "Content-type: text/javascript";
    else if (extension == "ico")  return "Content-type: image/x-icon";
    else if (extension == "png")  return "Content-type: image/png";
    
    return "";
}

void respond_file(Response *response, char* path)
{
    if (access(path, F_OK) == 0)
    {
        printf("[+]File access: %s\n", path);
        //* Body
        size_t file_size = read_file(path, &response->body);
        response->body_size = file_size;

        //* Headers
        char *token = strtok(path, ".");
        token = strtok(NULL, ".");
        response->headers = get_content_type_header(token);
        printf("%s\n", response->headers);
    }
    else
    {
        printf("[-]File doesn't exist: %s\n", path);
        respond_string(response, "File doesn't exit");
    }

}

Response build_response(Request request)
{
    Response response = {
        .code = 200,
        .status = "OK",
        .headers = "",
    };
    
    //* Requesting a file
    if (fnmatch("*.*", request.url, 0) == 0)
    {
        char *path = malloc(strlen(request.url + strlen("./root")));
        sprintf(path, "./root%s", request.url);
        respond_file(&response, path);
        free(path);
    }
    else
        respond_string(&response, "Hello!");

    return response;
}


size_t compile_response(u_int8_t buffer[BUFFER_SIZE], Response response)
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

    free(response.body);
    
    return size;
}