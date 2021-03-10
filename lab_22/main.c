#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>


#define FTW_F   1       
#define FTW_D   2       
#define FTW_DNR 3       
#define FTW_NS  4       


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


static int dopath(const char *filename, int n) 
{
    struct stat     statbuf;    
    struct dirent   *dirp;      
    DIR             *dp;         
    int             ret = 0;    


    if (lstat(filename, &statbuf) < 0)
        return(showFiles(filename, FTW_NS));

    for (int i = 0; i < n; i++)
        printf("______");

    if (S_ISDIR(statbuf.st_mode) == 0)  
        return(showFiles(filename, FTW_F));


    showFiles(filename, FTW_D);

    if ((dp = opendir(filename)) == NULL)
        return(showFiles(filename, FTW_DNR));

    chdir(filename);

    while ((dirp = readdir(dp)) != NULL && (ret == 0))
    {
        if (strcmp(dirp->d_name, ".") && strcmp(dirp->d_name, ".."))       
            ret = dopath(dirp->d_name, n + 1);
    }

    chdir("..");

    if (closedir(dp) < 0) 
        printf("ERROR: imposiible to close catalog %s", filename);

    return(ret);
}


int main(int argc, char *argv[])
{
    int ret;

    if (argc != 2)
    {
        printf("ERROR: wrong number of arguments (must be one - starting dir)\n");
        exit(1);
    }

    ret = dopath(argv[1], 0);
    
    return ret;
}
