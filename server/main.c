#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "./request/request.h"
#include "./response/response.h"

#define BUFFER_SIZE 1024

static const int port = 5566;

void create_socket(int *sock, struct sockaddr_in *addr)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(addr, '\0', sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(port);
}

void bind_socket(int *sock, struct sockaddr_in *addr)
{
    int n = bind(*sock, (struct sockaddr*)addr, sizeof(*addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

}

void read_file(char *path, char **buffer)
{
    FILE *f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    *buffer = malloc(fsize + 1);
    fread(*buffer, fsize, 1, f);
    fclose(f);
    (*buffer)[fsize] = 0;
}

int main(int argc, char const *argv[])
{
    int server_sock;
    struct sockaddr_in server_addr;
    
    create_socket(&server_sock, &server_addr);
    bind_socket(&server_sock, &server_addr);

    listen(server_sock, 1);
    printf("Listening...\n");


    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    while (1)
    {
        client_fd = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_fd < 0)
        {
            perror("[-]Accept failed");
            continue;
        }

        printf("[+]Client connected.\n");

        uint8_t buffer[BUFFER_SIZE] = {0};
        ssize_t length = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (length < 0)
        {   
            perror("[-]Request read");
            continue;
        }
        
        printf("%s\n", buffer);
        Request request = parse_request(buffer, length);

        Response response = {
            .code = 200,
            .status = "OK",
            .headers = "",
            .body = "Hello"
        };
        

        char *data;
        if (fnmatch("*.*", request.url, 0) == 0)
        {
            char *path = malloc(strlen(request.url + 1));
            sprintf(path, ".%s", request.url);

            if (access(path, F_OK) == 0)
            {
                printf("[+]File access: %s\n", path);
                read_file(path, &response.body);

                char *token = strtok(path, ".");
                token = strtok(NULL, ".");
                if (token == "html")
                    response.headers = "Content-type: text/html";
                else if (token == "css")
                    response.headers = "Content-type: text/css";
                else if (token == "js")
                    response.headers = "Content-type: text/javascript";
                else if (token == "ico")
                    response.headers = "Content-type: text/x-icon";
                
                data = build_response(response);
                free(response.body);

                
            }
            else
            {
                printf("[-]File doesn't exist: %s\n", path);
                response.body = "File doesn't exist";
                data = build_response(response);
            }

            free(path);
        }
        else
        {
            data = build_response(response);
        }

        memset(buffer, '\0', strlen(buffer));
        strcpy(buffer, data);

        free(data);

        int n = write(client_fd, buffer, strlen(buffer));
        close(client_fd);
    }



    return 0;
}
