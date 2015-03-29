#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>
int main(){
	pid_t pid=fork();
    if (pid==0) {
    	char *const *afg;
        execv("./lHelloWorld",afg);
        exit(127);
    }
    else { 
        waitpid(pid,0,0);
    }

}