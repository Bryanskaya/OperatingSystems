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
<<<<<<< HEAD
	pid_t childpid_1 = fork(), childpid_2;
	int status;

	if (childpid_1 == -1)
=======
	pid_t childpid = fork(), temp;
	int status;

	if (childpid == -1)
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
	{
		perror("Can't fork");
		exit(1);
	}
	
<<<<<<< HEAD
	if (childpid_1 == 0)			// Потомок 
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	}
	else							// Предок
	{
		childpid_2 = fork();
		
		if (childpid_2 == -1)
=======
	temp = childpid;
	
	if (childpid == 0)	// Потомок 
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	}
	else	// Предок
	{
		childpid = fork();
		
		if (childpid == -1)
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
		{
			perror("Can't fork");
			exit(1);
		}
		
<<<<<<< HEAD
		if (childpid_2 == 0)		// Потомок 
=======
		if (childpid == 0)	// Потомок 
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
		{
			printf("\nChild:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			return 0;
		}
		
<<<<<<< HEAD
		printf("Parent: id = %d	group_id  = %d \tchildren = %d %d\n", getpid(), getpgrp(), childpid_1, childpid_2);
=======
		printf("Parent: id = %d	group_id  = %d \tchildren = %d %d\n", getpid(), getpgrp(), temp, childpid);
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
		
		for (int i = 0; i < 2; i++)
		{
			printf("\n--- Parent is waiting ---");
<<<<<<< HEAD
			pid_t childpid = wait(&status);
=======
			childpid = wait(&status);
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
			printf("\nChild finished: pid = %d\n", childpid);
			
			if (WIFEXITED(status))
				printf("Child exited with code %d\n", WEXITSTATUS(status));
			else printf("Child terminated abnormally\n");
		}
		
	}

	return 0;
}

