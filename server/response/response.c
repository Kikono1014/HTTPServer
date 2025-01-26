#include "response.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>
#include <stdint.h>
#include <sys/stat.h>

#include "../consts.h"
#include "../request/request.h"

char *get_file_extension(char *filename)
{
    size_t i = strlen(filename) - 1;
    for (; i >= 0 && filename[i] != '.'; i--);
    
    return filename + i + 1;
}

size_t read_file(char *path, uint8_t **buffer)
{
    FILE *f = fopen(path, "rb");

    printf("1\n");
    struct stat *stat_buffer;
    printf("2\n");
    stat_buffer = malloc(sizeof(struct stat));
    printf("3\n");
    fstat(fileno(f), stat_buffer);
    printf("4\n");
    size_t fsize = stat_buffer->st_size;
    printf("5\n");
    free(stat_buffer);
    printf("6\n");

    printf("%zu\n", fsize);
    *buffer = malloc(fsize);

    fread(*buffer, fsize, 1, f);
    fclose(f);

    return fsize;
}

char *get_content_type_header(char *extension)
{
    if      (strcmp(extension, "html") == 0) return "Content-type: text/html";
    else if (strcmp(extension, "css") == 0)  return "Content-type: text/css";
    else if (strcmp(extension, "js") == 0)   return "Content-type: text/javascript";

    //* application
    else if (strcmp(extension, "jar") == 0)   return "application/java-archive";
    else if (strcmp(extension, "ogg") == 0)   return "application/ogg";
    else if (strcmp(extension, "pdf") == 0)   return "application/pdf";
    else if (strcmp(extension, "xhtml") == 0) return "application/xhtml+xml";
    else if (strcmp(extension, "json") == 0)  return "application/json";
    else if (strcmp(extension, "zip") == 0)   return "application/zip";

    //* audio
    else if (strcmp(extension, "mp3") == 0) return "audio/mpeg";
    else if (strcmp(extension, "wma") == 0) return "audio/x-ms-wma";
    else if (strcmp(extension, "wav") == 0) return "audio/x-wav";

    //* image
    else if (strcmp(extension, "gif") == 0)  return "image/gif";
    else if (strcmp(extension, "webp") == 0) return "image/webp";
    else if (strcmp(extension, "jpeg") == 0) return "image/jpeg";
    else if (strcmp(extension, "png") == 0)  return "image/png";
    else if (strcmp(extension, "tiff") == 0) return "image/tiff";
    else if (strcmp(extension, "ico") == 0)  return "image/x-icon";
    else if (strcmp(extension, "djvu") == 0) return "image/vnd.djvu";
    else if (strcmp(extension, "djv") == 0)  return "image/x-djvu";
    else if (strcmp(extension, "svg") == 0)  return "image/svg+xml";
    else if (strcmp(extension, "svgz") == 0) return "image/svg+xml";
    
    //* text
    else if (strcmp(extension, "csv") == 0)  return "text/csv";
    else if (strcmp(extension, "txt") == 0)  return "text/plain";
    else if (strcmp(extension, "text") == 0) return "text/plain";
    else if (strcmp(extension, "xml") == 0)  return "text/xml";

    //* video
    else if (strcmp(extension, "mp4") == 0)  return "video/mp4";
    else if (strcmp(extension, "mov") == 0)  return "video/quicktime";
    else if (strcmp(extension, "wmv") == 0)  return "video/x-ms-wmv";
    else if (strcmp(extension, "avi") == 0)  return "video/x-msvideo";
    else if (strcmp(extension, "flv") == 0)  return "video/x-flv";
    else if (strcmp(extension, "webm") == 0) return "video/webm";


    return "";
}


void respond_string(Response *response, char *string)
{
    //* Body
    response->body_size = strlen(string);
    response->body = malloc(response->body_size);
    strcpy(response->body, string);
}


void respond_file(Response *response, char* path)
{
    if (access(path, F_OK) == 0)
    {
        printf("[+]File access: %s\n", path);

        //* Headers
        // char *token = strtok(path, ".");
        // token = strtok(NULL, ".");
        char *extension = get_file_extension(path);
        response->headers = get_content_type_header(extension);
        printf("%s\n", extension);
        printf("%s\n", response->headers);
        printf("%s\n", path);
        
        
        //* Body
        size_t file_size = read_file(path, &response->body);
        response->body_size = file_size;
        // printf("%s\n", response->body);
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


size_t compile_response(u_int8_t *buffer, Response response)
{
    memset(buffer, 0, BUFFER_SIZE);
    sprintf(buffer, 
        "HTTP/1.1 %d %s\r\n%s\r\n\r\n",
        response.code, response.status,
        response.headers
    );
    size_t size = strlen(buffer);

    memcpy(buffer + size, response.body, response.body_size);
    size += response.body_size;

    free(response.body);
    
    return size;
}