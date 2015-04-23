#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int fd1[2];
int fd2[2];

int 
main(int argc, char *argv[])
{
    /* Pipe fd1 - for ls to grep */
    pipe(fd1);
    pid_t pid, pid2, pid3;
    pid = fork();

    /* Executing ls to fd1*/
    if(pid == -1)
    {
        perror("Blad");
    } 
    else if(pid == 0)
    {
        close(STDOUT_FILENO);
        close(fd1[0]);
        dup2(fd1[1], STDOUT_FILENO);
        execlp("ls", "ls","-l", NULL);
    }
    
    close(fd1[1]);

    /* fd2 - output for grep and input for wc */
    pipe(fd2);
    pid2 = fork();
    
    /* Execing grep */
    if(pid2 == -1)
    {
        perror("Blad");
    } 
    else if(pid2 == 0)
    {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);

        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);

        execlp("grep", "grep", "^d", NULL);
    }
    close(fd1[0]);
    close(fd2[1]);
    
    /* Execing wc */
    pid3 = fork();
    if(pid3 == -1)
    {
        perror("Blad");
    }
    else if(pid3 == 0)
    {
        close(STDIN_FILENO);
        dup2(fd2[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL);
    }

    close(fd2[0]);
    waitpid(pid3, NULL, 0);
    return 0;
}

