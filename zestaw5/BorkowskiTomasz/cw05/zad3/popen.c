#define _XOPEN_SOURCE   
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int 
main(int argc, char *argv[])
{
    /* Stream stdout to file folders.txt */
    freopen ("folders.txt", "w", stdout);

    /* Execute popen for reading */
    FILE *from = popen("ls -l", "r");

    /* Execute popen for writing */ 
    FILE *in = popen("grep ^d", "w");
    
    /* Reading from ls -l */
    char buff[10000];
    while(fgets(buff, 10000, from) != NULL)
        if(fputs(buff, in) == EOF)
            perror("Error popen");

    /* Closing all open files */
    pclose(in);
    pclose(from);

    return 0;
}

