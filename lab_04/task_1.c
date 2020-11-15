/*
Написать программу, запускающую не мене двух новых процессов системным вызовом fork(). 
В предке вывести собственный идентификатор (функция getpid()), 
идентификатор группы (функция getpgrp()) и идентификаторы потомков. 
В процессе-потомке вывести собственный идентификатор, 
идентификатор предка (функция getppid()) и идентификатор группы. 
Убедиться, что при завершении процесса-предка потомок, который продолжает выполняться,
получает идентификатор предка (PPID), равный 1 или идентификатор процесса-посредника.
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
<<<<<<< HEAD
	pid_t childpid_1 = fork(), childpid_2;

	if (childpid_1 == -1)
=======
	pid_t childpid = fork(), temp;

	if (childpid == -1)
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
	{
		perror("Can't fork");
		exit(1);
	}
	
<<<<<<< HEAD
	if (childpid_1 == 0)		// Потомок 
=======
	temp = childpid;
	
	if (childpid == 0)	// Потомок 
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
	{
		printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		sleep(1);
		printf("\nChild:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
	}
<<<<<<< HEAD
	else						// Предок
	{	
		childpid_2 = fork();
		
		if (childpid_2 == -1)
=======
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
		if (childpid_2 == 0)	// Потомок 
=======
		if (childpid == 0)	// Потомок 
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
		{
			printf("Child:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
			sleep(1);
			printf("\nChild:  id = %d \tparent_id = %d \tgroup_id = %d\n", getpid(), getppid(), getpgrp());
		}
		else
		{
<<<<<<< HEAD
			printf("Parent: id = %d	group_id  = %d \tchildren = %d %d\n", getpid(), getpgrp(), childpid_1, childpid_2);
=======
			printf("Parent: id = %d	group_id  = %d \tchildren = %d %d\n", getpid(), getpgrp(), temp, childpid);
>>>>>>> e75798cb3076656aad17e17538666aa8e2cbe0b1
		}
	}

	return 0;
}

