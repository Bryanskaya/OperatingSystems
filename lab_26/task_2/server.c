#include "includes.h"


#define SOCKET_NAME     "my_socket.soc"
#define SIZE            100


int sock;
int clients_arr[NUM];


void close_socket()
{
    for (int i = 0; i < NUM; i++)
        if (clients_arr[i])
            close(clients_arr[i]);
    
    close(sock);
}

int process_new_client()
{
    int new_sock = accept(sock, NULL, NULL);
    if (new_sock < 0) 
        return EXIT_FAILURE;

    printf("New connection added\n");

    for (int i = 0; i < NUM; i++) {
        if (!clients_arr[i]) 
        {
            clients_arr[i] = new_sock;
            break;
        }
    }

    return 0;
}

void process_client(int fd, int ind)
{
    char buf[SIZE];

    int rsize = recvfrom(fd, buf, SIZE, 0, NULL, NULL);
    if (!rsize)
    {
        printf("Client was disconnected\n");
        clients_arr[ind] = 0;
    }
    else
    {
        buf[rsize] = '\0';
        printf("Server got: %s\n", buf);
    } 
    
}

int main()
{
    struct sockaddr_in serv_addr;
    int max_fd, err, fd, new_sock, rsize;
    fd_set set;
    char buf[SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) 
    {
        perror("ERROR: socket failed");
        return EXIT_FAILURE;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        close(sock);
        perror("ERROR: bind failed");
        return EXIT_FAILURE;
    }

    if (listen(sock, NUM) < 0)
    {
        close(sock);
        perror("ERROR: listen failed");
        return EXIT_FAILURE;
    }

    printf("Server was created\n");

    while (1)
    {
        struct timeval interval = {30, 0};

        FD_ZERO(&set);
        FD_SET(sock, &set);
        

        max_fd = sock;

        for (int i = 0; i < NUM; i++)
        {
            if (clients_arr[i] > 0)
                FD_SET(clients_arr[i], &set);

            if (clients_arr[i] > max_fd)
                max_fd = clients_arr[i];
        }

        err = select(max_fd + 1, &set, NULL, NULL, &interval);
        if (err < 0)
        {
            close_socket();
            perror("ERROR: select failed");
            return EXIT_FAILURE;
        }
        else if (!err)
        {
            close_socket();
            printf("Time of waiting is over\n");
            return 0;
        }
        
        if (FD_ISSET(sock, &set))
        {
            new_sock = accept(sock, NULL, NULL);
            if (new_sock < 0) 
                return EXIT_FAILURE;

            printf("New connection added\n");

            for (int i = 0; i < NUM; i++) 
            {
                if (!clients_arr[i]) 
                {
                    clients_arr[i] = new_sock;
                    break;
                }
            }
        }

        for (int i = 0; i < NUM; i++)
        {
            fd = clients_arr[i];
            if ((fd > 0) && FD_ISSET(fd, &set))
            {
                rsize = recvfrom(fd, buf, SIZE, 0, NULL, NULL);
                if (!rsize)
                {
                    printf("Client was disconnected\n");
                    clients_arr[i] = 0;
                }
                else
                {
                    buf[rsize] = '\0';
                    printf("Server got: %s\n", buf);
                } 
            }
        }
    }

    close_socket();

    printf("Server was closed\n");

    return 0;
}