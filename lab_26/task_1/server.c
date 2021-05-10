#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


#define SOCKET_NAME     "my_socket.soc"
#define SIZE            100


int sock;


void close_socket()
{
    close(sock);
    unlink(SOCKET_NAME);
}

void close_by_signal(int signo)
{
    close_socket();
    exit(0);
}


int main()
{
    struct sockaddr srvr_name;
    char buf[SIZE];
    ssize_t len;

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) 
    {
        perror("ERROR: socket failed");
        return EXIT_FAILURE;
    }

    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCKET_NAME);

    if (bind(sock, &srvr_name, sizeof(srvr_name)) < 0) 
    {
        close(sock);
        perror("ERROR: bind failed");
        return EXIT_FAILURE;
    }

    signal(SIGINT, close_by_signal);

    printf("Server was created\n");

    while (1)
    {
        len = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
        if (len < 0)
        {
            close_socket();
            perror("ERROR: recvfrom failed");
            return EXIT_FAILURE;
        }

        buf[len] = '\n';
        buf[len + 1] = 0;
        printf("Server got: %s", buf);
    }

    close_socket();

    printf("Server was closed\n");

    return 0;
}