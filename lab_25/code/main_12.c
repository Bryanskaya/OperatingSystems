#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>


#define FILENAME    "alphabet.txt"
#define SIZE        20


void* thr_fn(void *arg)
{
    int fd = *((int*)arg);
    FILE *fs2 = fdopen(fd, "r");
    char buff2[SIZE];
    setvbuf(fs2, buff2, _IOFBF, SIZE);

    int flag2 = 1;
    while (flag2 == 1)
    {
        char c;

        flag2 = fscanf(fs2, "%c", &c);
        if (flag2 == 1)
            fprintf(stdout, "%c", c);
    }


    return 0;
}

int main()
{
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR: can't open file %d", fd);
        return -1;
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, thr_fn, (void*)(&fd)))
    {
        printf("ERROR: mistake in creating thread");
        return -1;
    }

    FILE *fs1 = fdopen(fd, "r");
    char buff1[SIZE];
    setvbuf(fs1, buff1, _IOFBF, SIZE);

    int flag1 = 1;
    
    while (flag1 == 1)
    {
        char c;

        flag1 = fscanf(fs1, "%c", &c);
        if (flag1 == 1)
            fprintf(stdout, "%c", c);
    }

    if (pthread_join(tid, NULL))
    {
        printf("ERROR: mistake in joining thread");
        return -1;
    }
    
    printf("\n");

    return 0;
}