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
<<<<<<< HEAD
int send_signal_1 = 0, send_signal_2 = 0;


void action(int sig_numb)
{	
	if (sig_numb == SIGTSTP)				// Ctrl-z
		send_signal_1 = 1;					
	else									// Ctrl-'\'
		send_signal_2 = 1;				
}

=======


void action_parent(int sig_numb)
{
	printf("Parent received signal number %d\n", sig_numb);
	
	if (sig_numb == SIGTSTP)
		kill(childpid_1, SIGUSR1);		// Ctrl-z
	else
		kill(childpid_2, SIGUSR2);		// Ctrl-'\'
}

void action_child_1(int sig_numb)
{
	printf("Child 1 received signal number %d\n", sig_numb); 
}

void action_child_2(int sig_numb)
{
	printf("Child 2 received signal number %d\n", sig_numb);
}


>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
int main(int argc, char *argv[])
{
	int fd[2];
	int status;
	char get_msg[25];
	char send_msg1[25], send_msg2[25];
	
<<<<<<< HEAD
	signal(SIGTSTP, action);				// Ctrl-z
	signal(SIGQUIT, action);				// Ctrl-'\'
			
=======
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
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
	
<<<<<<< HEAD
	if (childpid_1 == 0)				// Потомок 
=======
	if (childpid_1 == 0)	// Потомок 
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		
		close(fd[0]);
<<<<<<< HEAD
		sleep(1);
=======
		
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
		sprintf(send_msg1, "Hello, it's child %d", getpid());
		write(fd[1], send_msg1, sizeof(send_msg1));
		printf("\n- Child %d sent: %s\n", getpid(), send_msg1);
		
		close(fd[1]);	
		
<<<<<<< HEAD
		signal(SIGQUIT, SIG_IGN);		// Ctrl-'\'
		sleep(5);
					
		if (send_signal_1)
			printf("Child %d received signal\n", getpid()); 
	}
	else								// Предок
=======
		signal(SIGUSR1, action_child_1);
		signal(SIGQUIT, SIG_IGN);		// Ctrl-'\'
		signal(SIGTSTP, SIG_IGN);		// Ctrl-z
		sleep(5);
	}
	else	// Предок
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
	{
		printf("Parent: id = %d	group_id  = %d\n", getpid(), getpgrp());
		
		childpid_2 = fork();
		
		if (childpid_2 == -1)
		{
			perror("Can't fork");
			exit(1);
		}
		
<<<<<<< HEAD
		if (childpid_2 == 0)			// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			sleep(1);
=======
		if (childpid_2 == 0)	// Потомок 
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
			sprintf(send_msg2, "Hello, it's child %d", getpid());
			write(fd[1], send_msg2, sizeof(send_msg2));
			close(fd[1]);
			
			printf("\n- Child %d sent: %s\n", getpid(), send_msg2);
			
<<<<<<< HEAD
			signal(SIGTSTP, SIG_IGN);		// Ctrl-z
			sleep(5);
			
			if (send_signal_2)
				printf("Child %d received signal\n", getpid());
=======
			signal(SIGUSR2, action_child_2);
			signal(SIGQUIT, SIG_IGN);		// Ctrl-'\'
			signal(SIGTSTP, SIG_IGN);		// Ctrl-z
			sleep(5);
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1

			return 0;
		}
		
<<<<<<< HEAD
=======
		signal(SIGQUIT, action_parent);		// Ctrl-'\'
		signal(SIGTSTP, action_parent);		// Ctrl-z
		
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
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
