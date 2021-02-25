#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>


#define FTW_F   1       // Файл, не являющийся каталогом
#define FTW_D   2       // Каталог
#define FTW_DNR 3       // Каталог, который не доступен для чтения
#define FTW_NS  4       // Файл, информацию о котором невозможно получить с помощью stat


static char *fullpath; // Полный путь к каждому из файлов

// typedef int Myfunc(const char *,const struct stat *, int);
typedef int Myfunc(const char *, int);


static int dopath(const char *filename, Myfunc *func, int n) //filename - символьная ссылка на текущий файл
{
    struct stat     statbuf;    // структура с информацией о файле, тип, режим доступа, номер индексного узла и т.д.
    struct dirent   *dirp;      // в структуре dirent содержится номер индексного узла и имя файла
    DIR             *dp;        // структура, хранит информацию о каталоге, похожа на структуру FILE 
    int             ret;    

// lstat - работает как stat, только если 1ый аргумент - символическая ссылка, 
// то она возвращает сведения о самой ссылке, а не о файле, на который она ссылается
// 2ой аргумент - указатель на структуру, которую функция будет заполнять информацией
// в ней указаны права доступа, тип файла, количество ссылок, иеднтификатор пользователя и т.д.
// с помощью lstat можно увидеть символические ссылки
/*
    Символическая сслыка - косвенная ссылка на файл
    жёсткая - другое название файла, по прошлому семетру символьная - путь к файлу
    любой пользователь может создавать символическую ссылку на каталог
*/
    if (lstat(filename, &statbuf) < 0)
        return(func(filename, FTW_NS));

    for (int i = 0; i < n; i++)
        printf("______");

    if (S_ISDIR(statbuf.st_mode) == 0)  // S_ISDIR - макрос, каталог  //st_mode - хранит тип файла (биты прав доступа к файлу, 9 бит на 3 категории)
        return(func(filename, FTW_F));


    // Это каталог, сначала вызов func(), а затем обработка всех файлов в этом каталоге
    func(filename, FTW_D);

    /* opendir - инициализирует структуру dir, возвращает указатель в случае успеха (открывает поток каталога и возвращает указатель на этот поток, 
       поток устанавливается на первой записи в каталоге)*/
    if ((dp = opendir(filename)) == NULL)  // каталог недоступен
        return(func(filename, FTW_DNR));

    chdir(filename); // изменение текущего каталога
    /* readdir - считывает очередную запись, возвращает указатель на структуру dirent (на очередную запись)
       или пустой указатель, если всё прочитано*/
    while (dirp = readdir(dp)) //*
    {
        if (strcmp(dirp->d_name, ".") == 0 ||       /* d_name - имя файла*/
            strcmp(dirp->d_name, "..") == 0)        /* . - текущий каталог  .. - родительская директория*/
            continue;       // пропустить каталоги . и ..

        if ((ret = dopath(dirp->d_name, func, n + 1)))   // рекурсия, выход по ошибке
            break;
    }

    chdir("..");

    if (closedir(dp) < 0) // (закрывает поток, связанный с каталогом)
        printf("ERROR: imposiible to close catalog %s", filename);

    return(ret);
}



static int showFiles(const char *pathname, int type)
{
    switch (type)
    {
    case FTW_NS:
        printf("ERROR: mistake in call func lstat for %s\n", pathname);
        break;
    case FTW_F:
        printf(" %s\n", pathname);
        break;
    case FTW_D:
        printf(" %s/\n", pathname);
        break;
    case FTW_DNR:
        printf("ERROR: access to catalog %s is closed\n", pathname);
        break;
    
    default:
        printf("ERROR: unexpected type %d of file %s\n", type, pathname);
        break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int ret;

    if (argc != 2)
    {
        printf("ERROR: wrong number of arguments (must be one - starting dir)\n");
        exit(1);
    }

    ret = dopath(argv[1], showFiles, 0);
    
    return ret;
}

