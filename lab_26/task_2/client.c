#include "includes.h"


int main()
{
    int sock;
    struct hostent *server;
    struct sockaddr_in serv_addr;
    char buf[100];
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("ERROR: socket() failed");
        return EXIT_FAILURE;
    }

    server = gethostbyname(HOST);
    if (!server) {
        close(sock);
        perror("ERROR: gethostbyname() failed");
        return EXIT_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr = *((struct in_addr*) server->h_addr_list[0]);    
    serv_addr.sin_port = htons(PORT);
    
    if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        close(sock);
        perror("ERROR: connect() failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM; i++)
    {
        scanf("%s", buf);

        if (send(sock, buf, strlen(buf), 0) < 0)
        {
            close(sock);
            perror("ERROR: send() failed");
            return EXIT_FAILURE;
        }
        
        printf("Client sent: %s\n", buf);
    }

    return 0;
}