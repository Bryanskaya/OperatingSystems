/*
Написать программу по схеме первого задания, но в процессе-предке выполнить системный вызов wait(). Убедиться, что в этом случае идентификатор процесса потомка на 1 больше идентификатора процесса-предка.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


void child_action()
{
	printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
}

int main(int argc, char *argv[])
{
	pid_t childpid_1 = fork(), childpid_2;
	int status;

	if (childpid_1 == -1)
	{
		perror("Can't fork");
		exit(1);
	}

	if (childpid_1 == 0)
	{
		child_action();
		return 0;
	}
	
	childpid_2 = fork();

	if (childpid_2 == -1)
	{
		perror("Can't fork");
		exit(1);
	}

	if (childpid_2 == 0)
	{
		child_action();
		return 0;
	}

	printf("Parent: id = %d	group_id  = %d \tchildren = %d %d\n", getpid(), getpgrp(), childpid_1, childpid_2);

	for (int i = 0; i < 2; i++)
	{
		printf("\n--- Parent is waiting ---");
		
		pid_t childpid = wait(&status); // нужно ли отдельно смотреть wait ведь мы дальше его смотрим с макросами
		printf("\nChild finished: pid = %d\n", childpid);

		if (WIFEXITED(status))
			printf("Child exited with code %d\n", WEXITSTATUS(status));
		else printf("Child terminated abnormally\n");
	}

	return 0;
}

