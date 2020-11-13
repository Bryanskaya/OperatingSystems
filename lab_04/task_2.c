/*
Написать программу по схеме первого задания, но в процессе-предке выполнить системный вызов wait(). 
Убедиться, что в этом случае идентификатор процесса потомка на 1 больше идентификатора 
процесса-предка.
*/

#include <stdio.h>
#include <stdlib.h>

int main()
{
	pid_t childpid;

	if ((chiltpid = fork()) == -1)
	{
		perror("Can't fork");
		
		exit(1);
	}
	
	if (childpid == 0)	// Потомок 
	{
		//sleep(5);
		printf("Child: id = %d parent_id = %d group_id = %d ", getpid(), getppid(), getpgrp());
	}
	else (childpid == 1)	// Предок
	{
		printf("Parent: id = %d	group_id = %d children = %d\n", getpid(), getpgrp(), ******);
		wait();

		return 0;
	}
{