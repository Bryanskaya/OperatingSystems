#include <stdio.h>
#include <string.h>
#define BUF_SIZE    0x100 


#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>


#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>


void read_file_stat()
{
    char buf[BUF_SIZE + 100];
    char *stat_info[] = {
        ">>> pid", ">>> comm", ">>> state", "ppid", "pgrp", \
        "session", "tty", "tpgid", "flags", "minflt", \
        "cminflt", "majflt", "cmajflt", ">>> utime", ">>> stime", \
        "cutime", "cstime", "priority", "nice", "num_threads",\
        "itrealvalue", "starttime", "vsize", ">>> rss", "rsslim",\
        "startcode", "endcode", "startstack", "kstkesp", "kstkeip", \
        "signal", "blocked", "sigignore", "sigcatch", ">>> wchan", \
        "nswap", "cnswap", "exit_signal", "processor", "rt_priority", \
        "policy", "delayacct_blkio_ticks", "guest_time", "cguest_time", "start_data", \
        "end_data", "start_brk", "arg_start", "arg_end", "env_start",
        "end_end", "exit_code", NULL };

    FILE *f, *f_res;
    int i = 0;
    
    f = fopen("/proc/self/stat", "r");
    f_res = fopen("stat.txt", "w");

    fread(buf, 1, BUF_SIZE, f);

    char *temp = strtok(buf, " ");

    while (temp != NULL) {
        printf("%s:    %s\n", stat_info[i], temp);
        fprintf(f_res, "%s:    %s\n", stat_info[i], temp);
        i++;
        temp = strtok(NULL, " ");
    }

    fclose(f);
    fclose(f_res);
}


void read_mem()
{
    char buf[BUF_SIZE + 300];
    int len, i;
    int mem_fd = open("/proc/self/mem", O_RDONLY);

    lseek(mem_fd, BUF_SIZE - 1, SEEK_SET);
    printf("%ld\n\n", read(mem_fd, buf, BUF_SIZE));

    for(int i=0; i<BUF_SIZE; i++)
        printf("%d",buf[i]);
}

int main(int argc, char *argv[])
{
    struct dirent *dirp;
    FILE *f, *f_res;
    DIR *dp;
    char buf[BUF_SIZE];
    char path[BUF_SIZE + 30];

    int len, i;
    

    f = fopen("/proc/self/cmdline", "r");   //environ - файл, содержит окружения процесса
                                            //Данные о каждом процессе хранятся в поддиректории с именем,
                                            //которым является идентификатор процесса: /proc/<PID>.
                                            //id можно получить с помощью getpid() либо использовать ссылку self
    
    f_res = fopen("res.txt", "w");
    fprintf(f_res, "===================== cmdline ======================\n");

    fread(buf, 1, BUF_SIZE, f);
    fprintf(f_res, "%s\n\n", buf);

    fclose(f);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "======================= cwd ========================\n");

    readlink("/proc/self/cwd", buf, BUF_SIZE);
    fprintf(f_res, "%s\n\n", buf);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "===================== environ ======================\n");

    f = fopen("/proc/self/environ", "r"); 

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0) //считывает BUF_SIZE объектов по 1 символу в длину из f и помещает в buf
    {                                           //количество считанных объектов
        for (i = 0; i < len; i++)
            if (buf[i] == 0)    // 0 = \0 = конец строки
                buf[i] = 10;    // 10 = 0x0A = \n
        buf[len - 1] = 0;
        fprintf(f_res, "%s", buf);
    }

    fclose(f);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "\n\n======================= exe ========================\n");

    readlink("/proc/self/exe", buf, BUF_SIZE);
    fprintf(f_res, "%s\n\n", buf);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "======================== fd ========================\n");

    dp = opendir("/proc/self/fd");

    while ((dirp = readdir(dp)) != NULL) {
        if ((strcmp(dirp->d_name, ".") != 0) &&
            (strcmp(dirp->d_name, "..") != 0)) 
        {
            sprintf(path, "%s%s", "/proc/self/fd/", dirp->d_name);

            readlink(path, buf, BUF_SIZE);
            fprintf(f_res, "%s\t%s\n", dirp->d_name, buf);
        }
    }

    closedir(dp);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "\n======================= maps =======================\n");

    f = fopen("/proc/self/maps", "r"); 

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0) 
    {                                         
        for (i = 0; i < len; i++)
            if (buf[i] == 0) 
                buf[i] = 10;   
        buf[len - 1] = 0;
        fprintf(f_res, "%s", buf);
    }

    fclose(f);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "\n\n======================= mem ========================\n");

    read_mem();   //??????????


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "\n\n======================= root =======================\n");

    readlink("/proc/self/root", buf, BUF_SIZE);
    fprintf(f_res, "%s\n\n", buf);


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "======================= stat =======================\n");

    read_file_stat(); //????????


    memset(buf, 0, BUF_SIZE);
    fprintf(f_res, "\n\n======================= statm ======================\n");

    f = fopen("/proc/self/statm", "r");

    fread(buf, 1, BUF_SIZE, f);
    fprintf(f_res, "%s\n\n", buf);

    fclose(f);

    fclose(f_res);


    return 0;
}