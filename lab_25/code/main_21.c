#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define FILENAME    "alphabet.txt"


int main()
{
    char c;
    int fd1 = open(FILENAME, O_RDONLY);
    if (fd1 == -1)
    {
        printf("ERROR: can't open file %d", fd1);
        return -1;
    }

    int fd2 = open(FILENAME, O_RDONLY);
    if (fd2 == -1)
    {
        printf("ERROR: can't open file %d", fd2);
        return -1;
    }

    int flag1 = 1, flag2 = 1;

    while(flag1 && flag2)
    {
        flag1 = read(fd1,&c,1);
        if (flag1)
            write(1,&c,1);

        flag2 = read(fd2,&c,1);
        if (flag2)
            write(1,&c,1);
    }

    printf("\n");

    return 0;
}