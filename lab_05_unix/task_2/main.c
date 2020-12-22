#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define     SWR     0
#define     SWW     1
#define     SAR     2
#define     SAW     3

#define     NUM_W   3
#define     NUM_R   7


struct sembuf can_write_act[3] = 
{
    { SAR, 0, SEM_UNDO }, 
    { SAW, 0, SEM_UNDO },
    { SAW, 1, SEM_UNDO }
};

struct sembuf start_write_act[2] = 
{
    { SAW, 1, SEM_UNDO },
    { SWW, -1, SEM_UNDO }
};

struct sembuf stop_write_act[1] = 
{
    { SAW, -1, SEM_UNDO }
};

struct sembuf can_read_act[3] = 
{
    { SAW, 0, SEM_UNDO }, 
    { SAR, 0, SEM_UNDO },
    { SAR, 1, SEM_UNDO }
};

struct sembuf start_read_act[2] = 
{
    { SAR, 1, SEM_UNDO },
    { SWR, -1, SEM_UNDO }
};

struct sembuf stop_read_act[1] = 
{
    { SAR, -1, SEM_UNDO }
};


size_t get_len(struct sembuf arr[])
{
    return sizeof(*arr) / sizeof((arr)[0]);
}

void start_write(int id_sem)
{
    int temp = semop(id_sem, can_write_act, get_len(can_write_act));
    if (temp == -1)
    {
        perror("semop error");
        exit(6);
    }
	
    temp = semop(id_sem, start_write_act, get_len(start_write_act));
    if (temp == -1)
    {
        perror("semop error");
        exit(6);
    }
}

void stop_write(int id_sem)
{
    int temp = semop(id_sem, stop_write_act, get_len(stop_write_act));
    if (temp == -1)
    {
        perror("semop error");
        exit(6);
    }
}

void start_read(int id_sem)
{
    int temp = semop(id_sem, can_read_act, get_len(can_read_act));
    if (temp == -1)
    {
        perror("semop error");
        exit(6);
    }
	
    temp = semop(id_sem, start_read_act, get_len(start_read_act));
    if (temp == -1)
    {
        perror("semop error");
        exit(6);
    }
}

void stop_read(int id_sem)
{
    int temp = semop(id_sem, stop_read_act, get_len(stop_read_act));
    if (temp == -1)
    {
        perror("semop error");
         exit(6);
    }
}


void action_writer(int cur_id, int id_sem, int* addr)
{
    sleep(rand() % 2 + 1);

    while (1)
    {
        start_write(id_sem);

        (*addr)++;
        printf(">> WRITER %d: wrote %d\n", cur_id, *addr);

        stop_write(id_sem);

        sleep(rand() % 2 + 1);
    }
}

void action_reader(int cur_id, int id_sem, int* addr)
{
    sleep(rand() % 3 + 1);

    while (1)
    {
        start_read(id_sem);

        printf(">> READER %d: read %d\n", cur_id, *addr);

        stop_read(id_sem);

        sleep(rand() % 3 + 1);
    }
}

int main()
{
    srand(time(NULL));

    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int swr, sww, sar, saw, res;
    pid_t writers[NUM_W], readers[NUM_R];

    int id_sem = semget(IPC_PRIVATE, 4, IPC_CREAT | perms);
    if (id_sem == -1)
    {
        perror("semget error");
        exit(1);
    }

    swr = semctl(id_sem, SWR, SETVAL, 0);
    sww = semctl(id_sem, SWW, SETVAL, 0);
    sar = semctl(id_sem, SAR, SETVAL, 0);
    saw = semctl(id_sem, SAW, SETVAL, 0);

    if (swr == -1 || sww == -1 || sar == -1 || saw == -1)
    {
        perror("semctl error");
        exit(2);
    }

    int id_shm = shmget(IPC_PRIVATE, 1 * sizeof(int), IPC_CREAT | perms);
    if (id_shm == -1)
    {
        perror("shmget error");
        exit(3);
    }

    int* addr = (int*)shmat(id_shm, 0, 0);
    if (addr == (int*)-1)
    {
        perror("shmat error");
        exit(4);
    }

    *addr = -1;

    for (int i = 0; i < NUM_W; i++)
    {
        writers[i] = fork();
        if (writers[i] == -1)
        {
            perror("fork error");
            exit(5);
        }

        if (writers[i] == 0)
            action_writer(i, id_sem, addr);

        rand();
    }

    for (int i = 0; i < NUM_R; i++)
    {
        readers[i] = fork();
        if (readers[i] == -1)
        {
            perror("fork error");
            exit(5);
        }

        if (readers[i] == 0)
            action_reader(i, id_sem, addr);

        rand();
    }

    int status, temp_id;
    for (int i = 0; i < NUM_R + NUM_W; i++)
    {
        temp_id = wait(&status);
        
        if (temp_id == -1)
        {
            perror("wait error");
            exit(7);
        }
    }

    if (shmctl(id_shm, IPC_RMID, NULL) == -1)
    {
        perror("shmctl error");
        exit(8);
    }

    if (shmctl(id_sem, 0, IPC_RMID) == -1)
    {
        perror("shmctl error");
        exit(8);
    }

    return 0;
}