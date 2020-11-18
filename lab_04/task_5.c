/*
В программу с программным каналом включить собственный обработчик сигнала. 
Использовать сигнал для изменения хода выполнения программы.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*sighandler_t)(int);


pid_t childpid_1, childpid_2;
int send_signal_1 = 0, send_signal_2 = 0;


void action(int sig_numb)
{	
	if (sig_numb == SIGTSTP)				// Ctrl-z
		send_signal_1 = 1;					
	else									// Ctrl-'\'
		send_signal_2 = 1;				
}


int main(int argc, char *argv[])
{
	int fd[2];
	int status;
	char get_msg[25];
	char send_msg1[25], send_msg2[25];
	
	if (signal(SIGTSTP, action) == SIG_ERR)				// Ctrl-z
	{
		printf("Signal error");
		exit(1);
	}
	
	if (signal(SIGQUIT, action) == SIG_ERR)				// Ctrl-'\'
	{
		printf("Signal error");
		exit(1);
	}
			
	if (pipe(fd) == -1)
	{
		perror("Can't pipe");
		exit(1);
	}
	
	childpid_1 = fork();

	if (childpid_1 == -1)
	{
		perror("Can't fork");
		exit(1);
	}
	
	if (childpid_1 == 0)
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		
		if (close(fd[0]) == -1)
		{
			printf("Close error");
			exit(1);
		}
		
		sleep(1);
		sprintf(send_msg1, "Hello, it's child %d", getpid());
		status = write(fd[1], send_msg1, sizeof(send_msg1));
		if (status == -1)
		{
			printf("Write error");
			exit(1);
		}
		
		printf("\n- Child %d sent: %s\n", getpid(), send_msg1);
		
		if (close(fd[1]) == -1)
		{
			printf("Close error");
			exit(1);
		}
		
		if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)		// Ctrl-'\'
		{
			printf("Signal error");
			exit(1);
		}
		
		sleep(5);
					
		if (send_signal_1)
			printf("Child %d received signal\n", getpid()); 
			
		return 0;
	}

	printf("Parent: id = %d	group_id  = %d\n", getpid(), getpgrp());
	
	childpid_2 = fork();
	
	if (childpid_2 == -1)
	{
		perror("Can't fork");
		exit(1);
	}
	
	if (childpid_2 == 0)
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		sleep(1);
		sprintf(send_msg2, "Hello, it's child %d", getpid());
		status = write(fd[1], send_msg2, sizeof(send_msg2));
		if (status == -1)
		{
			printf("Write error");
			exit(1);
		}
		
		if (close(fd[1]) == -1)
		{
			printf("Close error");
			exit(1);
		}
		
		printf("\n- Child %d sent: %s\n", getpid(), send_msg2);
		
		if (signal(SIGTSTP, SIG_IGN) == SIG_ERR)		// Ctrl-z
		{
			printf("Signal error");
			exit(1);
		}
		
		sleep(5);
		
		if (send_signal_2)
			printf("Child %d received signal\n", getpid());

		return 0;
	}
	
	for (int i = 0; i < 2; i++)
	{
		pid_t childpid = wait(&status);// WAIT
		printf("\nChild finished: pid = %d\n", childpid);
		
		if (WIFEXITED(status))
			printf("Child exited with code %d\n", WEXITSTATUS(status));
		else printf("Child terminated abnormally\n");
	}
	
	if (close(fd[1]) == -1)
	{
		printf("Close error");
		exit(1);
	}
	
	status = read(fd[0], get_msg, sizeof(send_msg1));
	if (status == -1)
	{
		printf("Read error");
		exit(1);
	}
	
	printf("\n--> Parent read: %s\n", get_msg);
	
	status = read(fd[0], get_msg, sizeof(send_msg2));
	if (status == -1)
	{
		printf("Read error");
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
