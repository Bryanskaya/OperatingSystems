#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define SOCKET_NAME     "my_socket.soc"


int main()
{
    int sock;
    struct sockaddr srvr_name;
    char buf[100];
    ssize_t err;
    
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("ERROR: socket failed");
        return EXIT_FAILURE;
    }

    
    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCKET_NAME);

    scanf("%s", buf);

    err = sendto(sock, buf, strlen(buf), 0, &srvr_name, sizeof(srvr_name));
    if (err < 0)
    {
        close(sock);
        perror("ERROR: socket failed");
        return EXIT_FAILURE;
    }

    printf("Client sent: %s\n", buf);

    close(sock);

    return 0;
}