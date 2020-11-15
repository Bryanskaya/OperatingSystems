/*
Написать программу, запускающую не мене двух новых процессов системным вызовом fork(). 
В предке вывести собственный идентификатор (функция getpid()), 
идентификатор группы (функция getpgrp()) и идентификаторы потомков. 
В процессе-потомке вывести собственный идентификатор, 
идентификатор предка (функция getppid()) и идентификатор группы. 
Убедиться, что при завершении процесса-предка потомок, который продолжает выполняться,
получает идентификатор предка (PPID), равный 1 или идентификатор процесса-посредника.
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	pid_t childpid_1 = fork(), childpid_2;

	if (childpid_1 == -1)
	{
		perror("Can't fork");
		exit(1);
	}
	
	if (childpid_1 == 0)		// Потомок 
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		sleep(1);
		printf("\nChild:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	}
	else						// Предок
	{	
		childpid_2 = fork();
		
		if (childpid_2 == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		
		if (childpid_2 == 0)	// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			sleep(1);
			printf("\nChild:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		}
		else
		{
			printf("Parent: id = %d	group_id  = %d \tchildren = %d %d\n", getpid(), getpgrp(), childpid_1, childpid_2);
		}
	}

	return 0;
}

