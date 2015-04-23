#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>


/* Fun to get time in format: HH:MM:SS:NNNNNN
where NNNNNN are miliseconds 
*/ 
char *get_time()
{
    struct timeval mytime;
    gettimeofday(&mytime, NULL);
    char *res = (char *) ctime(&(mytime.tv_sec));
    char *hours = calloc(17, sizeof(char));
    strncpy(hours, res+11, 8);
    char *mili = calloc(7, sizeof(char));
    sprintf(mili,"%d", (int) mytime.tv_usec/1000 );
    strcat(hours, ":");
    strcat(hours, mili);
    return hours;
}

int
main(int argc, char *argv[])
{   
    if(argc != 2)
    {
        printf("[rdfifo] Too few arguments\n");
        exit(EXIT_FAILURE);
    }

    char *fifo_name = argv[1];

    /* Delete old fifo if exists */
    if(access(fifo_name, F_OK) != -1)
    {
	    if(remove(fifo_name) != 0)
		    perror("Error deleting file");
	  	else
		    puts("File successfully deleted");
	}


    /* Creating fifo */
    if(mkfifo(fifo_name,  S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH) != 0)
    {
        perror("[rdfifo]  mkfifo error\n\t");
        exit(EXIT_FAILURE);
    }

    /*Opening fifo */
    int fifo;
    if((fifo = open(fifo_name, O_RDONLY)) < 0)
    {
    	perror("[rdfifo] Error opening fifo");
		exit(EXIT_FAILURE);
    }

    /* Waitng for write to file */
    while(1)
    {
    	int n;     // number of read bytes 
      	char *buffer;
    	
    	/* How big is next message */
    	int to_read = 100;
    	if((n = read(fifo, &to_read, sizeof(int))) < 0)
    	{
    		perror("[rdfifo] Error reading fifo: ");
 			continue;
    	}

    	/* Read message */
    	buffer = (char *) malloc(to_read * sizeof(char));

    	n = read(fifo, buffer, to_read * sizeof(char));
        /* If message size is bigger than 2 */
        if(n > 2)
        {
            char *current_time = get_time();
	    	printf("%s %s", current_time, buffer);
        }
        
        /*
         Sleep a second to not use to much processor
         and prevent to be killed by system 
        */
        sleep(1);
    }
	exit(0);
}
 
