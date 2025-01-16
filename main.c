#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>


static const char *ip = "127.0.0.1";
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
    addr->sin_port = port;
    addr->sin_addr.s_addr = inet_addr(ip);
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


static int server_sock;
static struct sockaddr_in server_addr;
static uint8_t buffer[1024];

int main(int argc, char const *argv[])
{
    create_socket(&server_sock, &server_addr);
    bind_socket(&server_sock, &server_addr);

    listen(server_sock, 5);
    printf("Listening...\n");


    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        printf("[+]Client connected.\n");

        bzero(buffer, 1024);
        recv(client_sock, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);
        close(client_sock);
    }



    return 0;
}
