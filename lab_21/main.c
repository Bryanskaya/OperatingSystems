// Реализовать демона, который выводит имя пользователя и время (задание 13.3)
// На основе листинга 13.3

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/file.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

sigset_t mask;

int lockfile(int fd)
{
    struct flock f;

    f.l_type = F_WRLCK; 
    f.l_whence = SEEK_SET; 
    f.l_start = 0; 
    f.l_len = 0;

    return fcntl(fd, F_SETLK, &f);
}


int already_running()
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE); 
    if (fd == -1)
    {
        syslog(LOG_ERR, "ERROR: impossible to open %s: %s", LOCKFILE, strerror(errno)); 
        exit(1);
    }

    if (lockfile(fd) == -1)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            return 1;
        }

        syslog(LOG_ERR, "ERROR: impossible to make lock on %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    ftruncate(fd, 0);   
    sprintf(buf, "%ld\n", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return(0);
}

void daemonize(const char *cmd)
{
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;


    umask(0); 

    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) 
        perror("ERROR: impossible to get max number of the descriptor\n");

    if ((pid = fork()) == -1)
        perror("ERROR: fork()\n");
    else if (pid != 0)
    {
        printf("***** Child PID %d *****\n", pid);
        exit(0);
    }

    setsid();

    sa.sa_handler = SIG_IGN; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) == -1)
        perror("ERROR: impossible to ignore signal SIGHUP");

    if (chdir("/") == -1)
        perror("ERROR: impossible to make / a current directory");

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (int i = 0; i < rl.rlim_max; i++)
        close(i);

    fd0 = open("/dev/null", O_RDWR);   
    fd1 = dup(0);
    fd2 = dup(0);   

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "ERROR: mistake in file's descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    } 
} 

void* thr_fn(void *arg)
{
    int err, signo;
    time_t scnd = time(NULL);

    for (;;)
    {
        err = sigwait(&mask, &signo);
        if (err)
        {
            syslog(LOG_ERR, "ERROR: mistake in call function sigwait");
            exit(1);
        }

        switch (signo)
        {
        case SIGHUP:
            syslog(LOG_INFO, ">>> INFO: USER'S NAME: %s    TIME: %s", getlogin(), asctime(localtime(&scnd)));
            break;
        case SIGTERM:
            syslog(LOG_INFO, "INFO: received signal SIGTERM. EXIT");
            exit(0);
        default:
            syslog(LOG_INFO, "INFO: received unexpected signal");
        }
    }

    return(0);
}


int main(int argc, char *argv[])
{
    char *cmd;
    int err;
    pthread_t tid;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;

    daemonize(cmd);

    if (already_running())
    {
        syslog(LOG_ERR, "ERROR: daemon has already launched");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) == -1)
    {
        syslog(LOG_ERR, "ERROR: mistake in recovering SIG_DFL for SIGHUP");
        exit(1);
    }


    sigfillset(&mask); 

    if (pthread_sigmask(SIG_BLOCK, &mask, NULL))
    {
        syslog(LOG_ERR, "ERROR: mistake with SIG_BLOCK");
        exit(1);
    }

    if (pthread_create(&tid, NULL, thr_fn, 0))
    {
        syslog(LOG_ERR, "ERROR: mistake in creating thread");
        exit(1);
    }

    if (pthread_join(tid, NULL))
    {
        syslog(LOG_ERR, "ERROR: mistake in joining thread");
        exit(1);
    }

    exit(0);
}