#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


void run_server()
{
    int pid = fork();
    if (pid == 0)
    {
        int err;
        char *argv[3] = { 0 };
        err = execl("./bin/server", NULL);
        exit(err);
    }
    else if (pid < 0)
    {
        printf("fork failed with error code %d\n", pid);
        exit(-1);
    }

    int status;
    wait(&status);
    printf("Server with pid %d exited: %s\n", pid, strerror(status));
}

int main()
{
    run_server();
    return 0;
}