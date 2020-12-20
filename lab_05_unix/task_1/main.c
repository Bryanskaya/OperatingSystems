#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>

#define     SHM_SIZE    1024 //???
#define     N           10 //????
#define     NUM_PR      3

#define     SE          0
#define     SF          1
#define     SB          2


struct sembuf prd_before_putting_in_buf[2] = 
{
    {SE, 1, SEM_UNDO},
    {SB, 1, SEM_UNDO}
};

struct sembuf prd_after_putting_in_buf[2] = 
{
    {SB, -1, SEM_UNDO},
    {SF, -1, SEM_UNDO}
};

struct sembuf cns_before_taking_from_buf[2] = 
{
    {SF, 1, SEM_UNDO},
    {SB, 1, SEM_UNDO}
};

struct sembuf cns_after_taking_from_buf[2] = 
{
    {SB, -1, SEM_UNDO},
    {SE, -1, SEM_UNDO}
};


int action_producer(int cur_id, int n, char* addr)
{
    int temp;

    while (1)
    {
        temp = semop(cur_id, prd_before_putting_in_buf, 2);
        if (temp == -1)
        {
            perror("semop error");
            return 6;
        }

        // n += 1

        temp = semop(cur_id, prd_after_putting_in_buf, 2);
        if (temp == -1)
        {
            perror("semop error");
            return 6;
        }

        //

    }

    return 0;
}

int action_consumer(int cur_id, int n, char* addr)
{
    int temp;

    while (1)
    {
        temp = semop(cur_id, cns_before_taking_from_buf, 2);
        if (temp == -1)
        {
            perror("semop error");
            return 6;
        }

        // n += 1

        temp = semop(cur_id, cns_after_taking_from_buf, 2);
        if (temp == -1)
        {
            perror("semop error");
            return 6;
        }

        //

    }

    return 0;
}


int main()
{
    int perms = S_IRWXU | S_IRWXG | S_IRWXO;    // Право на чтение/запись и выполнение для владельца-пользователя | тоже самое только для группы | для остальных пользователей
    int se, sf, sb, res;
    pid_t producers[NUM_PR], consumers[NUM_PR];

    int id_shm = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | perms); // Создаем разделяемый сегмент памяти, возв идентификатор
    if (id_shm == -1)
    {
        perror("shmget error");
        exit(1);
    }

    char* addr = (char*)shmat(id_shm, 0, 0); // shmat возвращает указатель на сегмент, первый 0 - система сама выбирает подходящий адрес, второй 0 - должен быть флаг
    if (addr == (char*)-1) // int или char
    {
        perror("shmat error");
        exit(2);
    }

    int id_sem = semget(IPC_PRIVATE, 3, IPC_CREAT | perms);
    if (id_sem == -1)
    {
        perror("semget error");
        exit(3);
    }

    se = semctl(id_sem, SE, SETVAL, N);
    sf = semctl(id_sem, SF, SETVAL, 0);
    sb = semctl(id_sem, SB, SETVAL, 1);

    if (se == -1 || sf == -1 || sb == -1)
    {
        perror("semctl error");
        exit(4);
    }
    
    for (int i = 0; i < NUM_PR; i++)
    {
        producers[i] = fork();
        if (producers[i] == -1)
        {
            perror("fork error");
            exit(5);
        }

        if (producers[i] == 0)
        {
            res = action_producer(id_sem, N, addr);
            if (res)
                exit(res);
        }

        consumers[i] = fork();
        if (consumers[i] == -1)
        {
            perror("fork error");
            exit(5);
        }

        if (consumers[i] == 0)
        {
            res = action_consumer(id_sem, N, addr);
            if (res)
                exit(res);
        }
            

        // что-то с задержками
    }



    return 0;
}