#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
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

void bind_socket(int sock, struct sockaddr_in *addr)
{
    int n = bind(sock, (struct sockaddr*)addr, sizeof(*addr));
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
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        
    create_socket(&server_sock);
    bind_socket(server_sock, &server_addr);

    listen(server_sock, 1);
    printf("Listening...\n");
    
    return server_sock;
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


    uint8_t *buffer = malloc(BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
    while (1)
    {
        //* Establishing connection
        client_fd = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_fd < 0) {
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
        
        
        //* Building response
        Response response = build_response(request);
        size_t response_size = compile_response(buffer, response);

        //* Sending response
        int n = write(client_fd, buffer, response_size);
        
        //TODO deal with connections
        close(client_fd);

        free(request.headers);
        free(request.body);

        memset(buffer, 0, BUFFER_SIZE);
    }



    return 0;
}
