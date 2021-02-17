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
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) // права достипа: пользователь на чтение, запись; группа на чтение; остальные на чтение

sigset_t mask;

int lockfile(int fd)
{
    struct flock f;

    f.l_type = F_WRLCK; // F_WRLCK - блокировка на запись - такую блокировку может удерживать только один процесс (а на чтение несколько)
    f.l_whence = SEEK_SET; // смещение от начала файла
    f.l_start = 0;  // начальное смещение для блокировки, которое интерпретируется как: начало файла (если значение l_whence установлено в SEEK_SET)
    f.l_len = 0;    /* неотрицательное целое число, которое задаёт количество байт, которые будут заблокированы. 
                       Значение 0 для l_len имеет специальное назначение: блокировка всех байтов, начиная от позиции, заданной l_whence и l_start до конца файла,
                       не зависимо от того, насколько велик файл */

    return fcntl(fd, F_SETLK, &f);  // F_SETLK - установить блокировку, которая описана в типе, если занято, то вернёт -1
}

// запуск только одной копии демона
int already_running()
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE); // O_RDWR - открыть для чтения и записи, O_CREAT - если файл не существует, то он будет создан, возвращает новый  дескриптор файла
    if (fd == -1)
    {
        syslog(LOG_ERR, "ERROR: impossible to open %s: %s", LOCKFILE, strerror(errno)); // LOG_ERR - уровень важности (4ый от самого важного)
        exit(1);
    }

    if (lockfile(fd) == -1) // что вернется в случае успеха
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            return 1;
        }

        syslog(LOG_ERR, "ERROR: impossible to make lock on %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    ftruncate(fd, 0);   // усечение размера файла - удаление инфы, относящейся к предыдущей копии демона
    sprintf(buf, "%ld\n", (long)getpid()); // запись идентификатора демона в файл
    write(fd, buf, strlen(buf) + 1);

    return(0);
}

void daemonize(const char *cmd)
{
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;


    umask(0);   // сбросить маску режима создания файлов

    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) // получить макимально возможный номер дескриптора,  максимальное число файлов, которые могут быть одновременно открыты процессом
        perror("ERROR: impossible to get max number of the descriptor\n");

    // стать лидером новой сессии, чтобы утратить управляющий терминал 
    if ((pid = fork()) == -1)
        perror("ERROR: fork()\n");
    else if (pid != 0)  // родительский процесс
    {
        printf("***** Child PID %d *****\n", pid);
        exit(0);
    }

    setsid();   // создать новую сессию

    // Обеспечение невозможности обретения терминала в будущем 
    sa.sa_handler = SIG_IGN;    // игнорим 
    sigemptyset(&sa.sa_mask); // установка маски сигналов
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) == -1)
        perror("ERROR: impossible to ignore signal SIGHUP");

    // в учебнике еще раз fork

    // назначить корневой каталог текущим рабочим каталогом, чтобы впоследствии можно было отмонтировать файловую систему 
    if (chdir("/") == -1)
        perror("ERROR: impossible to make / a current directory");

    // закрыть все открытые файловые дескрипторы
    if (rl.rlim_max == RLIM_INFINITY)   // RLIM_INFINITY определяет отсутствие ограничений для ресурса
        rl.rlim_max = 1024;
    for (int i = 0; i < rl.rlim_max; i++)
        close(i);

    // присоединить файловые дескрипторы 0, 1, 2 к /dev/null
    /* дескриптор 0 - STDIN (ввод данных)
       дескриптор 1 - STDOUT (вывод данных)
       дескриптор 2 - STDERR (вывод данных, сообщающих об ошибке)*/

    fd0 = open("/dev/null", O_RDWR);    /* /dev/null — специальный файл, представляющий собой так называемое «пустое устройство». 
                                            Запись в него происходит успешно, независимо от объёма «записанной» информации.*/
    fd1 = dup(0); //копируем файловый дискриптор
    fd2 = dup(0);   

    // Инициализировать файл журнала
    openlog(cmd, LOG_CONS, LOG_DAEMON); /* устанавливает связь с программой, ведущей системный журнал
                                           1 арг - добавляется к каждому сообщению и обычно представляет собой название программы
                                           2 - указывает флаг управляющий работой openlog() и соответствующих вызовов syslog()
                                           3 - тип программы, которая выводит сообщение
                                           
                                           LOG_CONS написать сообщение об ошибке в консоли, если ошибка при записи данных в системный журнал
                                           LOG_DAEMON - сообщения от других демонов системы*/
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

    // убеждаемся, что ранее не была запущена другая копия демона
    if (already_running())
    {
        syslog(LOG_ERR, "ERROR: daemon has already launched");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;    // выполнение действий по умолчанию - в большинстве случаев - окончание процесса.
    sigemptyset(&sa.sa_mask); // установка маски сигналов
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) == -1)
    {
        syslog(LOG_ERR, "ERROR: mistake in recovering SIG_DFL for SIGHUP");
        exit(1);
    }

    sigfillset(&mask);  //полностью инициализирует набор set, в котором содержатся все сигналы

    if (pthread_sigmask(SIG_BLOCK, &mask, NULL))    // добавьте набор сигналов к набору заблокированных сигналов
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