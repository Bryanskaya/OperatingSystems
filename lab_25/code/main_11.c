#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define FILENAME    "alphabet.txt"
#define SIZE        20

int main()
{
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1)
    {
        printf("ERROR: can't open file %d", fd);
        return -1;
    }

    FILE *fs1 = fdopen(fd, "r");
    char buff1[SIZE];
    setvbuf(fs1, buff1, _IOFBF, SIZE);

    FILE *fs2 = fdopen(fd, "r");
    char buff2[SIZE];
    setvbuf(fs2, buff2, _IOFBF, SIZE);

    int flag1 = 1, flag2 = 1;
    

    while (flag1 == 1 || flag2 == 1)
    {
        char c;

        flag1 = fscanf(fs1, "%c", &c);
        if (flag1 == 1)
            fprintf(stdout, "fs1: %c\n", c);

        flag2 = fscanf(fs2, "%c", &c);
        if (flag2 == 1)
            fprintf(stdout, "fs2: %c\n", c);
    }

    return 0;
}