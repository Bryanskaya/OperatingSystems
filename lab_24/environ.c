#include <stdio.h>
#define BUF_SIZE    0x100   // 256

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    int len, i;
    FILE *f, *f_res;

    f = fopen("/proc/self/environ", "r");   //environ - файл, содержит окружения процесса
                                            //Данные о каждом процессе хранятся в поддиректории с именем,
                                            //которым является идентификатор процесса: /proc/<PID>.
                                            //id можно получить с помощью getid() либо использовать ссылку self

    f_res = fopen("result.txt", "w");

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0) //считывает BUF_SIZE объектов по 1 символу в длину из f и помещает в buf
    {                                              //количество считанных объектов
        for (i = 0; i < len; i++)
            if (buf[i] == 0)    // 0 = \0 = конец строки
                buf[i] = 10;    // 10 = 0x0A = \n
        buf[len - 1] = 0;
        printf("%s", buf);
        fprintf(f_res, "%s", buf);
    }

    fclose(f);
    fclose(f_res);

    return 0;
}