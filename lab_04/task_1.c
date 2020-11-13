/*
Написать программу, запускающую не мене двух новых процессов системным вызовом fork(). 

В предке вывести собственный идентификатор (функция getpid()), 
идентификатор группы (функция getpgrp()) и идентификаторы потомков. 

В процессе-потомке вывести собственный идентификатор, 
идентификатор предка (функция getppid()) и идентификатор группы. 

Убедиться, что при завершении процесса-предка потомок, который продолжает выполняться,
получает идентификатор предка (PPID), равный 1 или идентификатор процесса-посредника.
*/

#include <stdio.h>
#include <stdlib.h>

int main()
{
	pid_t childpid;

	if ((chiltpid = fork()) == -1)
	{
		//sleep(5);
		perror("Can't fork");
		
		exit(1);
	}
	
	if (childpid == 0)	// Потомок 
	{
		printf("Child: id = %d parent_id = %d group_id = %d ", getpid(), getppid(), getpgrp());

		return 0;
	}
	else (childpid == 1)	// Предок
	{
		printf("Parent: id = %d	group_id = %d children = %d\n", getpid(), getpgrp(), ******);
		
		return 0;
	}
{