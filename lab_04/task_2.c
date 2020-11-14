/*
Написать программу по схеме первого задания, но в процессе-предке выполнить системный вызов wait(). Убедиться, что в этом случае идентификатор процесса потомка на 1 больше идентификатора процесса-предка.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	pid_t childpid = fork();
	int status;

	if (childpid == -1)
	{
		perror("Can't fork");
		exit(1);
	}
	
	if (childpid == 0)	// Потомок 
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	}
	else	// Предок
	{
		printf("Parent: id = %d	group_id  = %d \tchildren = %d\n", getpid(), getpgrp(), childpid);
		
		childpid = fork();
		
		if (childpid == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		
		if (childpid == 0)	// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			return 0;
		}
		
		for (int i = 0; i < 2; i++)
		{
			childpid = wait(&status);
			printf("\nChild finished: pid = %d\n", childpid);
			
			if (WIFEXITED(status))
				printf("Child exited with code %d\n", WEXITSTATUS(status));
			else printf("Child terminated abnormally\n");
		}
		
	}

	return 0;
}

