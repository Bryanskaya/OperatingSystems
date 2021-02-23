#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FTW_F   1       // Файл, не являющийся каталогом
#define FTW_D   2       // Каталог
#define FTW_DNR 3       // Каталог, который не доступен для чтения
#define FTW_NS  4       // Файл, информацию о котором невозможно получить с помощью stat


static char *fullpath; // Полный путь к каждому из файлов


static int myftw(char *pathname, Myfunc *func)
{
    // в учебнике происходит выделение памяти, но в репах его нет нигде

    return (dopath(func)); 
}


static int dopath(Myfunc *func)
{
    struct stat     statbuf;
    struct main     *dirp; // нет в репе
    DIR             *dp;
    int             ret;
    char            *ptr; // нет

    if (lstat(fullpath, &statbuf) < 0)
        return(func(fullpath, &statbuf, FTW_NS));

    if (S_ISDIR(statbuf.st_mode) == 0)  // не каталог
        return(func(fullpath, &statbuf, FTW_F));


    // Это каталог, сначала вызов func(), а затем обработка всех файлов в этом каталоге
    if (ret = func(fullpath, &statbuf, FTW_D))
        return(ret);

    ptr = fullpath + strlen(fullpath); // установить указатель в конец fullpath //нет
    *ptr++ = '/';
    *ptr = 0;

    if ((dp = opendir(fullpath)) == NULL)  // каталог недоступен
        return(func(fullpath, &statbuf, FTW_DNR));

    while (dirp = readdir(dp))
    {
        if (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;       // пропустить каталоги . и ..
        
        strcpy(ptr, dirp->d_name);

        if ((ret = dopath(func)))   // рекурсия, выход по ошибке
            break;
    }

    ptr[-1] = 0; // стереть часть строки от слэша и до конца

    if (closedir(dp) < 0)
        perror("ERROR: imposiible to close catalog %s", fullpath);

    return(ret);
}

static int myfunc()
{
    // подсчет файлов, нам не надо

    return(0);
}

int main(int argc, char *argv[])
{
    int ret;

    if (argc != 2)
    {
        perror("ERROR: wrong number of arguments (must be one - starting dir)");
        exit(1);
    }

    ret = myftw(argv[1], myfunc);
    
    return 0;
}

