#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{	
	if (argc != 2)
	{
		printf("\n\n--> Error: wrong number of arguments!\n");
		exit(1);
	}
	
	printf("\n\n--> Hello, my id is %s\n", argv[1]);
	
	return 0;
}
