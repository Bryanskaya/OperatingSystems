#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NUM_WRITERS     3
#define NUM_READERS     6


HANDLE mtx;
HANDLE can_write;
HANDLE can_read;

HANDLE writers[NUM_WRITERS];
HANDLE readers[NUM_READERS];

BOOL active_writer; //boolean
int active_readers; // = 0?


void start_write()
{
    //InterlockedIncrement16

    if (active_writer || )
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

DWORD WINAPI action_writer(int cur_id) // в методе DWORD  // как подать переменную
{
    while (1)
    {
        start_write();

        //мутки с мьютексом
        sh_var++;
        printf(">>> WRITER %d: wrote %d\n", cur_id, sh_var);
        //мутки с мьютексом
        stop_write();
        Sleep(300);

    }

    return 0;
}

DWORD WINAPI action_reader(int cur_id)
{
    while (1)
    {
        start_read();
        //мутки с мьютексом
        printf(">>> READER %d: read %d\n", cur_id, sh_var);
        //мутки с мьютексом
        stop_read();
        Sleep(300);

    }

    return 0;
}


void create_mutex()
{
    mtx = CreateMutex(NULL, FALSE, NULL);
    if (mtx == NULL)
    {
        perror("CreateMutex error\n");
        exit(1);
    }
}

void create_events()
{
    can_write = CreateEvent(NULL, FALSE, TRUE, NULL);
    can_read = CreateEvent(NULL, TRUE, TRUE, NULL);
    if (can_write == NULL || can_read == NULL)
    {
        perror("CreateEvent error\n");
        exit(2);
    }
}

void create_threads()
{
    for (int i = 0; i < NUM_WRITERS; i++)
    {
        writers[i] = CreateThread(NULL, 0, action_writer(i), mtx, 0, NULL);
        if (writers[i] == NULL)
        {
            perror("CreateThread error\n");
            exit(3);
        }
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        readers[i] = CreateThread(NULL, 0, action_reader(i), mtx, 0, NULL);
        if (readers[i] == NULL)
        {
            perror("CreateThread error\n");
            exit(3);
        }
    }
}




int main()
{
    create_mutex();
    create_events();
    create_threads();

    //что делать с разделяемой переменной




    printf("****");


    CloseHandle(mtx);
    CloseHandle(can_read);
    CloseHandle(can_write);

    return 0;
}
