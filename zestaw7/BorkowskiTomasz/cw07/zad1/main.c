#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int 
main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Usage: <%s> <-p | -c> \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if(!strcmp(argv[1], "-p"))
		execl("./producer", "producer", (char *) 0);
	else
		execl("./consumer", "consumer", (char *) 0);

	exit(EXIT_FAILURE);
}	