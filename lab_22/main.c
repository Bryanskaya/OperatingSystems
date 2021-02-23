#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>


#define FTW_F   1       // Файл, не являющийся каталогом
#define FTW_D   2       // Каталог
#define FTW_DNR 3       // Каталог, который не доступен для чтения
#define FTW_NS  4       // Файл, информацию о котором невозможно получить с помощью stat


static char *fullpath; // Полный путь к каждому из файлов

typedef int Myfunc(const char *,const struct stat *, int);


static int dopath(Myfunc *func)
{
    struct stat     statbuf;
    struct dirent   *dirp;
    DIR             *dp;
    int             ret;
    char            *ptr; // нет

// lstat - работает как stat, только если 1ый аргумент - символическая ссылка, 
// то она возвращает сведения о самой ссылке, а не о файле, на который она ссылается
// 2ой аргумент - указатель на структуру, которую функция будет заполнять информацией
// в ней указаны права доступа, тип файла, количество ссылок, иеднтификатор пользователя и т.д.
// с помощью lstat можно увидеть символические ссылки
/*
    Символическая сслыка (возможно по Рязановой символьная) - косвенная ссылка на файл
    жёсткая - другое название файла, по прошлому семетру символьная - путь к файлу
    любой пользователь может создавать символическую ссылку на каталог
*/
    if (lstat(fullpath, &statbuf) < 0)
        return(func(fullpath, &statbuf, FTW_NS));

    if (S_ISDIR(statbuf.st_mode) == 0)  // S_ISDIR - каталог  //st_mode - хранит тип файла (биты прав доступа к файлу, 9 бит на 3 категории)
        return(func(fullpath, &statbuf, FTW_F));


    // Это каталог, сначала вызов func(), а затем обработка всех файлов в этом каталоге
    if (ret = func(fullpath, &statbuf, FTW_D))
        return(ret);

    ptr = fullpath + strlen(fullpath); // установить указатель в конец fullpath //нет
    *ptr++ = '/';
    *ptr = 0;

    /* opendir - возвращает указатель в случае успеха (открывает поток каталога и возвращает указатель на этот поток, 
       поток устанавливается на первой записи в каталоге)*/
    if ((dp = opendir(fullpath)) == NULL)  // каталог недоступен
        return(func(fullpath, &statbuf, FTW_DNR));

    /* readdir - считывает очередную запись, возвращает указатель на структуру dirent
       или пустой указатель, если всё прочитано*/
    while (dirp = readdir(dp))
    {
        if (strcmp(dirp->d_name, ".") == 0 ||       /* d_name - имя файла*/
            strcmp(dirp->d_name, "..") == 0)        /* . - текущий каталог  .. - родительская директория*/
            continue;       // пропустить каталоги . и ..
        
        strcpy(ptr, dirp->d_name);

        if ((ret = dopath(func)))   // рекурсия, выход по ошибке
            break;
    }

    ptr[-1] = 0; // стереть часть строки от слэша и до конца

    if (closedir(dp) < 0) // (закрывает поток, связанный с каталогом)
        printf("ERROR: imposiible to close catalog %s", fullpath);

    return(ret);
}


static int myftw(char *pathname, Myfunc *func)
{
    // в учебнике происходит выделение памяти, но в репах его нет нигде

    return (dopath(func)); 
}


static int showFiles(const char *pathname, const struct stat *statptr, int type)
{
    // подсчет файлов, нам не надо
    switch (type)
    {
    case FTW_NS:
        printf("ERROR: mistake in call func lstat for %s\n", pathname);
        break;
    case FTW_F:
        printf("__ %s\n", pathname);
        break;
    case FTW_D:
        printf("__ %s/\n", pathname);
        break;
    case FTW_DNR:
        printf("ERROR: access to catalog %s is closed\n", pathname);
        break;
    
    default:
        printf("ERROR: unexpected type %d of file %s\n", type, pathname);
        break;
    }

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

    ret = myftw(argv[1], showFiles);
    
    return ret;
}

