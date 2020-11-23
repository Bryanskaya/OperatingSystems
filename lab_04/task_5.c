/*
В программу с программным каналом включить собственный обработчик сигнала. 
Использовать сигнал для изменения хода выполнения программы.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


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
	char get_msg[25], send_msg[25];
	pid_t childpid_1, childpid_2;
	
	if (signal(SIGTSTP, action) == SIG_ERR || signal(SIGQUIT, action) == SIG_ERR)    // Ctrl-z и Ctrl-'\'
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
		
		if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)		// Ctrl-'\'
		{
			printf("Signal error");
			exit(1);
		}
			
		if (close(fd[0]) == -1)
		{
			printf("Close error");
			exit(1);
		}
	
		sleep(5);
		
		if (send_signal_1)
		{
			sprintf(send_msg, "Hello, it's child %d", getpid());
			status = write(fd[1], send_msg, sizeof(send_msg));
			if (status == -1)
			{
				printf("Write error");
				exit(1);
			}
			
			printf("\n- Child %d sent: %s\n", getpid(), send_msg);
		}
		
		if (close(fd[1]) == -1)
		{
			printf("Close error");
			exit(1);
		}
	
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
		
		if (signal(SIGTSTP, SIG_IGN) == SIG_ERR)		// Ctrl-'\'
		{
			printf("Signal error");
			exit(1);
		}
			
		if (close(fd[0]) == -1)
		{
			printf("Close error");
			exit(1);
		}
		
		sleep(5);
		
		if (send_signal_2)
		{
			sprintf(send_msg, "Hello, it's child %d", getpid());
			status = write(fd[1], send_msg, sizeof(send_msg));
			if (status == -1)
			{
				printf("Write error");
				exit(1);
			}
			
			printf("\n- Child %d sent: %s\n", getpid(), send_msg);
		}
		
		if (close(fd[1]) == -1)
		{
			printf("Close error");
			exit(1);
		}

		return 0;
	}
	
	for (int i = 0; i < 2; i++)
	{
		pid_t childpid = wait(&status);
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
	
	if (send_signal_1)
	{
		status = read(fd[0], get_msg, sizeof(get_msg));
		if (status == -1)
		{
			printf("Read 1 error");
			exit(1);
		}
		
		printf("\n--> Parent read: %s\n", get_msg);
	}
	
	if (send_signal_2)
	{
		status = read(fd[0], get_msg, sizeof(get_msg));
		if (status == -1)
		{
			printf("Read 2 error");
			exit(1);
		}
		
		printf("\n--> Parent read: %s\n", get_msg);
	}
	
	if (close(fd[0]) == -1)
	{
		printf("Close error");
		exit(1);
	}

	return 0;
}
