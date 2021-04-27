#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>


#define FILENAME    "alphabet_3.txt"

char c[] = "abcdefghijklmnopqrstuvwxyzABC";


void* thr_fn(void *arg)
{
    int err = *((int*)arg);
    struct stat sbuf;

    FILE* fd2 = fopen(FILENAME, "w");
    if (!fd2)
    {
        printf("ERROR: can't open file for 2nd time");
        err = -1;
        return 0;
    }
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);

    int i = 0;

    while(i < strlen(c))
    {
        fprintf(fd2, "%c", c[i]);
        i += 2;
    }

    fclose(fd2);
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);

    return 0;
}


int main()
{
    struct stat sbuf;
    int err = 0;

    pthread_t tid;
    if (pthread_create(&tid, NULL, thr_fn, (void*)(&err)))
    {
        printf("ERROR: in creating thread");
        return -1;
    }
    
    FILE* fd1 = fopen(FILENAME, "w");
    if (!fd1)
    {
        printf("ERROR: can't open file for 1st time");
        return -1;
    }
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);


    int i = 1;

    while(i < strlen(c))
    {
        fprintf(fd1, "%c", c[i]);
        i += 2;
    }

    fclose(fd1);
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);

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