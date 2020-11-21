/*
Написать программу, в которой предок и потомок обмениваются сообщением через программный канал.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int child_action(int fd[])
{
	char send_msg[25];
	int status;

	printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	
	if (close(fd[0]) == -1)
	{
		printf("Close error");
		return -1;
	}
	
	sprintf(send_msg, "Hello, it's child %d", getpid());
	status = write(fd[1], send_msg, sizeof(send_msg));
	if (status == -1)
	{
		printf("Write error");
		return -1;
	}
	
	if (close(fd[1]) == -1)
	{
		printf("Close error");
		return -1;
	}
	
	sleep(1);
	printf("\n- Child %d sent: %s\n", getpid(), send_msg);
	
	return 0;
}

int main(int argc, char *argv[])
{
	int fd[2];
	int status;
	char get_msg[25];
	
	if (pipe(fd) == -1)
	{
		perror("Can't pipe");
		exit(1);
	}
	
	pid_t childpid = fork();

	if (childpid == -1)
	{
		perror("Can't fork");
		exit(1);
	}
	
	if (childpid == 0) 
	{
		status = child_action(fd);
		if (status)
			exit(1);
		
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
		status = child_action(fd);
		if (status)
			exit(1);
		
		return 0;
	}
	
	for (int i = 0; i < 2; i++)
	{
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
	
	if (close(fd[1]) == -1)
	{
		printf("Close error");
		exit(1);
	}
	
	status = read(fd[0], get_msg, sizeof(get_msg));
	if (status == -1)
	{
		printf("Read 1 error");
		exit(1);
	}
	
	printf("\n--> Parent read: %s\n", get_msg);
	
	status = read(fd[0], get_msg, sizeof(get_msg));
	if (status == -1)
	{
		printf("Read 2 error");
		exit(1);
	}
	
	printf("\n--> Parent read: %s\n", get_msg);
	
	if (close(fd[0]) == -1)
	{
		printf("Close error");
		exit(1);
	}

	return 0;
}

