/*
В программу с программным каналом включить собственный обработчик сигнала. 
Использовать сигнал для изменения хода выполнения программы.
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


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
	
	signal(SIGTSTP, action);				// Ctrl-z
	signal(SIGQUIT, action);				// Ctrl-'\'
			
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
	
	if (childpid_1 == 0)				// Потомок 
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		
		close(fd[0]);
		sleep(1);
		sprintf(send_msg1, "Hello, it's child %d", getpid());
		write(fd[1], send_msg1, sizeof(send_msg1));
		printf("\n- Child %d sent: %s\n", getpid(), send_msg1);
		
		close(fd[1]);	
		
		signal(SIGQUIT, SIG_IGN);		// Ctrl-'\'
		sleep(5);
					
		if (send_signal_1)
			printf("Child %d received signal\n", getpid()); 
	}
	else								// Предок
	{
		printf("Parent: id = %d	group_id  = %d\n", getpid(), getpgrp());
		
		childpid_2 = fork();
		
		if (childpid_2 == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		
		if (childpid_2 == 0)			// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			sleep(1);
			sprintf(send_msg2, "Hello, it's child %d", getpid());
			write(fd[1], send_msg2, sizeof(send_msg2));
			close(fd[1]);
			
			printf("\n- Child %d sent: %s\n", getpid(), send_msg2);
			
			signal(SIGTSTP, SIG_IGN);		// Ctrl-z
			sleep(5);
			
			if (send_signal_2)
				printf("Child %d received signal\n", getpid());

			return 0;
		}
		
		for (int i = 0; i < 2; i++)
		{
			pid_t childpid = wait(&status);
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
