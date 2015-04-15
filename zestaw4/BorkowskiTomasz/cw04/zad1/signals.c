#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>   
	
void signal_callback_handler(int);

int coutner = 0;
int main(int argc, char *argv[]){

	pid_t pid;


	if((pid = fork()) >= 0){
		if(pid == 0 ){
			signal(SIGUSR1, signal_callback_handler);
		
			while(1){
				sleep(1);
 			}

 		} else {
 			while(1){
 				sleep(1);
 				kill(pid, SIGUSR1);
 			}
 		}	
	} else 
		printf("Fork error\n");

}

void signal_callback_handler(int signumber){
	
	if(signumber == SIGUSR1){
		coutner++;
		printf("Counter: %d\n", coutner);
	}
	signal(SIGUSR1, signal_callback_handler);

}
