#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>

#define     SWR     0
#define     SWW     1
#define     SAR     2
#define     SAW     3

#define     NUM_W   3
#define     NUM_R   5


struct sembuf can_write_act[3] = 
{
    { SAR, 0, IPC_NOWAIT }, 
    { SAW, 0, IPC_NOWAIT },
    { SAW, 1, 0 }
};

struct sembuf start_write_act[2] = 
{
    { SAW, 1, 0 },
    { SWW, -1, 0}
};

struct sembuf stop_write_act[1] = 
{
    { SAW, -1, 0 }
};

struct sembuf can_read_act[3] = 
{
    { SAW, 0, IPC_NOWAIT }, 
    { SAR, 0, IPC_NOWAIT },
    { SAR, 1, 0 }
};

struct sembuf start_read_act[2] = 
{
    { SAR, 1, 0 },
    { SWR, -1, 0}
};

struct sembuf stop_read_act[1] = 
{
    { SWR, -1, 0 }
};




size_t get_len(struct sembuf arr[])
{
    return sizeof(arr) / sizeof((arr)[0]);
}

void start_write(int id_sem)
{
    int temp = semop(id_sem, can_write_act, get_len(can_write_act));
    if (temp == -1)
    {
        // действия при ожидании
        //perror("semop error");
        return 6;
    }
	
    temp = semop(id_sem, start_write_act, get_len(start_write_act));
    if (temp == -1)
    {
        perror("semop error");
        return 6;
    }
}

void stop_write()
{

}

void start_read()
{

}

void stop_read()
{

}


int action_writer(int cur_id, int id_sem, int* addr)
{
    while (1)
    {
        sleep(rand() % 4 + 1);

        start_write(id_sem);

        (*addr)++; //точто так?
        printf(">> WRITER %d: wrote %d\n", cur_id, *addr);

        stop_write();
    }

    return 0;
}

int action_reader(int cur_id, int id_sem, int* addr)
{
    while (1)
    {
        sleep(rand() % 4 + 1);

        start_read();

        printf(">> READER %d: read %d\n", cur_id, *addr);

        stop_read();
    }

    return 0;
}

int main()
{
    srand(time(NULL));

    int perms = S_IRWXU | S_IRWXG | S_IRWXO;
    int swr, sww, sar, saw, res;
    pid_t writers[NUM_W], readers[NUM_R];

    int id_sem = semget(IPC_PRIVATE, 4, IPC_CREAT | perms); // 5
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
    if (addr == -1)
    {
        perror("shmat error");
        exit(4);
    }

    for (int i = 0; i < NUM_W; i++)
    {
        writers[i] = fork();
        if (writers[i] == -1)
        {
            perror("fork error");
            exit(5);
        }

        if (writers[i] == 0)
        {
            res = action_writer(i, id_sem, addr);
            if (res)
                exit(res);
        }
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
        {
            res = action_reader(i, addr);
            if (res)
                exit(res);
        }
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

    if (shmctl(id_sem, 0, IPC_RMID) == -1) //почему у него только saw вместо 0
    {
        perror("shmctl error");
        exit(8);
    }

    return 0;
}