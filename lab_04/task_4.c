/*
Написать программу, в которой предок и потомок обмениваются сообщением через программный канал.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd[2];
	int status;
	char get_msg[25];
	char send_msg1[25], send_msg2[25];
	
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
	
	if (childpid == 0)	// Потомок 
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		
		close(fd[0]);
		
		sprintf(send_msg1, "Hello, it's child %d", getpid());
		write(fd[1], send_msg1, sizeof(send_msg1));
		printf("\n- Child %d sent: %s\n", getpid(), send_msg1);
		
		close(fd[1]);	
	}
	else	// Предок
	{
		printf("Parent: id = %d	group_id  = %d\n", getpid(), getpgrp());
		
		childpid = fork();
		
		if (childpid == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		
		if (childpid == 0)	// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			
			sprintf(send_msg2, "Hello, it's child %d", getpid());
			write(fd[1], send_msg2, sizeof(send_msg2));
			close(fd[1]);
			
			printf("\n- Child %d sent: %s\n", getpid(), send_msg2);
			
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
		
		close(fd[1]);
		
		read(fd[0], get_msg, sizeof(send_msg1));
		printf("\n--> Parent read: %s\n", get_msg);
		
		read(fd[0], get_msg, sizeof(send_msg2));
		printf("\n--> Parent read: %s\n", get_msg);
		
		close(fd[0]);
	}

	return 0;
}

