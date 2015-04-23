#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* 
Fun to get time in format: HH:MM:SS:NNNNNN
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
        printf("[wrfifo] Too few arguments\n");
        exit(EXIT_FAILURE);
    }

    /* Opening fifo */
    int fifo;
    char *fifo_name = argv[1];

    if((fifo = open(fifo_name, O_WRONLY)) < 0)
    {
        perror("[wrfifo] Can't open fifo");
        exit(EXIT_FAILURE);
    }

    /* Reading message */
    char buff[100];
    fgets(buff, 100, stdin);

    /* Getting pid to message */
    char message[120];
    sprintf(message, "%d ", (int)getpid());


    /* Getting time */
    char *hour = get_time();
    strcat(message, hour);
    strcat(message, "\n\t");
    
    /* Add text to message */
    strcat(message, buff);
    strcat(message, "\n");

    int message_length = strlen(message + 1);
    
    /* Write len of message */
    write(fifo, &message_length, sizeof(int));

    /* Write message to fifo */
    write(fifo, message, message_length * sizeof(char));

    close(fifo);

    exit(0);
}
 
