#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#define FILENAME    "alphabet_3.txt"


int main()
{
    struct stat sbuf;
    char c[] = "abcdefghijklmnopqrstuvwxyzABC";


    FILE* fd1 = fopen(FILENAME, "w");
    if (!fd1)
    {
        printf("ERROR: can't open file for 1st time");
        return -1;
    }
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);


    FILE* fd2 = fopen(FILENAME, "w");
    if (!fd2)
    {
        printf("ERROR: can't open file for 2nd time");
        return -1;
    }
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);


    int i = 0;

    while(i < strlen(c))
    {
        if (i % 2)
            fprintf(fd2, "%c", c[i]);
        else
            fprintf(fd1, "%c", c[i]);
        
        i += 1;
    }

    fclose(fd2);
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);

    fclose(fd1);
    stat(FILENAME, &sbuf);
    printf("\n>>> inode: %d\t size: %d\n", (int)sbuf.st_ino, (int)sbuf.st_size);


    return 0;
}