/*
Написать программу, в которой процесс-потомок вызывает системный вызов exec(), а процесс-предок ждет завершения процесса-потомка. Следует создать не менее двух потомков.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int child_action_1()
{
	char temp[10];
	int status;

	printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	
	sprintf(temp, "%d", getpid());
	status = execl("add_program_1.o", "add_program_1.o", temp, NULL);
	
	return status;
}

int child_action_2()
{
	char temp[10];
	int status;

	printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	
	status = execl("add_program_2.o", "", NULL);
	
	return status;
}


int main(int argc, char *argv[])
{
	pid_t childpid = fork();
	int status;

	if (childpid == -1)
	{
		perror("Can't fork");
		exit(1);
	}

	if (childpid == 0)		
	{
		status = child_action_1();
		if (status == -1)
		{
			printf("Execl error");
			exit(1);
		}
		
		return 0;
	}

	printf("Parent: id = %d	group_id  = %d\n", getpid(), getpgrp());

	childpid = fork();

	if (childpid == -1)
	{
		perror("Can't fork");
		exit(1);
	}

	if (childpid == 0)
	{	
		status = child_action_2();
		if (status == -1)
		{
			printf("Execl error");
			exit(1);
		}
		
		return 0;
	}

	for (int i = 0; i < 2; i++)
	{
		printf("\n--- Parent is waiting ---");
		
		childpid = wait(&status);
		if (childpid == -1)
		{
			if (errno == ECHILD)
				printf("Process does not have any unwaited for children\n");
			else if (errno == EINTR)
				printf("Call interrupted by signal\n");
			else if (errno == EINVAL)
				printf("Wrong argument\n");
			exit(1);
		}
		
		printf("\nChild finished: pid = %d\n", childpid);

		if (WIFEXITED(status))
			printf("Child exited normally with code %d\n", WEXITSTATUS(status));
		else printf("Child terminated abnormally\n");
		
		if (WIFSIGNALED(status))
			printf("Child exited due to uncaught signal # %d\n", WTERMSIG(status));
			
		if (WIFSTOPPED(status))
			printf("Child stopped, signal # %d\n", WSTOPSIG(status));
	}

	return 0;
}

