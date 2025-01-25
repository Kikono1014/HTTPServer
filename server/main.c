#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>
#include <signal.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "./request/request.h"
#include "./response/response.h"
#include "./consts.h"

int server_sock = 0;

void create_socket(int *sock)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

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

int setup_socket()
{
    struct sockaddr_in server_addr;

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    
    create_socket(&server_sock);
    bind_socket(&server_sock, &server_addr);

    listen(server_sock, 1);
    printf("Listening...\n");

    return server_sock;
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


void cleanup()
{
    printf("Cleaning\n");
    close(server_sock);
}
void interrupt(int error) { exit(error); }


int main(int argc, char const *argv[])
{
    atexit(cleanup);
    signal(SIGINT, interrupt);

    setup_socket();

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    uint8_t buffer[BUFFER_SIZE] = {0};

    while (1)
    {
        //* Establishing connection
        client_fd = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_fd < 0)
        {
            perror("[-]Accept failed");
            continue;
        }

        printf("[+]Client connected.\n");


        //* Receiving request
        ssize_t length = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (length < 0)
        {   
            perror("[-]Request read failed");
            continue;
        }
        
        printf("Request:\n");
        printf("%s\n\n", buffer);


        Request request = parse_request(buffer, length);


        Response response = {
            .code = 200,
            .status = "OK",
            .headers = "",
        };
        write_string(&response, "Hello!");

        
        //* Building response
        if (fnmatch("*.*", request.url, 0) == 0)
        {
            char *path = malloc(strlen(request.url + strlen("./root")));
            sprintf(path, "./root%s", request.url);

            printf("%s\n", path);

            if (access(path, F_OK) == 0)
            {
                printf("[+]File access: %s\n", path);
                size_t file_size = read_file(path, &response.body);
                response.body_size = file_size;


                char *token = strtok(path, ".");
                token = strtok(NULL, ".");
                if (token == "html")
                    response.headers = "Content-type: text/html";
                else if (token == "css")
                    response.headers = "Content-type: text/css";
                else if (token == "js")
                    response.headers = "Content-type: text/javascript";
                else if (token == "ico")
                    response.headers = "Content-type: image/x-icon";
                else if (token == "png")
                    response.headers = "Content-type: image/png";
                


            }
            else
            {
                printf("[-]File doesn't exist: %s\n", path);
                write_string(&response, "File doesn't exit");
            }

            free(path);
        }

        size_t response_size = build_response(buffer, response);

        free(response.body);


        //* Sending response
        int n = write(client_fd, buffer, response_size);
        close(client_fd);

        memset(buffer, 0, BUFFER_SIZE);
    }



    return 0;
}
