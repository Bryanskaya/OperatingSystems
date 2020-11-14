/*
Написать программу, в которой предок и потомок обмениваются сообщением через программный канал.
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd[2];
	char get_msg1[20], get_msg2[20];
	char send_msg[] = "Hello, it's parent";
	
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
		
		close(fd[1]);
		
		read(fd[0], get_msg1, sizeof(send_msg));
		close(fd[0]);	
		
		printf("--> Child %d read: %s\n", getpid(), get_msg1);
	}
	else	// Предок
	{
		printf("Parent: id = %d	group_id  = %d\n", getpid(), getpgrp());
		
		write(fd[1], send_msg, sizeof(send_msg));
		printf("- Parent send: %s\n", send_msg);
		
		write(fd[1], send_msg, sizeof(send_msg));
		printf("- Parent send: %s\n", send_msg);
		
		close(fd[1]);
		
		childpid = fork();
		
		if (childpid == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		
		if (childpid == 0)	// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			
			read(fd[0], get_msg2, sizeof(send_msg));
			
			printf("--> Child %d read: %s\n", getpid(), get_msg2);
		}
		
		close(fd[0]);
	}

	return 0;
}

