#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#define FILENAME    "alphabet.txt"


void* thr_fn(void *arg)
{
    char c;
    int err = *((int*)arg);
    int fd2 = open(FILENAME, O_RDONLY);
    if (fd2 == -1)
    {
        printf("ERROR: can't open file %d", fd2);
        err = -1;
        return 0;
    }

    int flag2 = 1;

    while(flag2)
    {
        flag2 = read(fd2,&c,1);
        if (flag2)
            write(1,&c,1);
    }

    return 0;
}

int main()
{
    char c;
    int err = 0;

    pthread_t tid;
    if (pthread_create(&tid, NULL, thr_fn, (void*)(&err)))
    {
        printf("ERROR: in creating thread");
        return -1;
    }

    int fd1 = open(FILENAME, O_RDONLY);
    if (fd1 == -1)
    {
        printf("ERROR: can't open file %d", fd1);
        return -1;
    }

    int flag1 = 1;
    while(flag1)
    {
        flag1 = read(fd1,&c,1);
        if (flag1)
            write(1,&c,1);
    }

    if (pthread_join(tid, NULL))
    {
        printf("ERROR: in joining thread");
        return -1;
    }
    if (err)
    {
        printf("ERROR: in reading in thread");
        return -1;
    }
    

    return 0;
}