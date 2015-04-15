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
void final_callback_handler(int);
void send_signals();


int coutner = 0;

int main(int argc, char *argv[]){

	printf("[Receiver] Started, my parent pid is %d and my pid is %d\n", getppid(), getpid());

	/* Preparet to receive signals */
	struct sigaction sig_act_usr1, sig_act_usr2;
	sig_act_usr1.sa_handler = signal_callback_handler;
	sig_act_usr1.sa_flags = 0;

	sig_act_usr2.sa_handler = final_callback_handler;
	sig_act_usr2.sa_flags = 0;

	if(sigaction(SIGUSR1, &sig_act_usr1, NULL) == -1 ||
		sigaction(SIGUSR2, &sig_act_usr2, NULL) == -1){
		perror("Problem occurred in sigaction");
		return 2;
	}

	sigset_t empty;
	sigemptyset(&empty);

	while(1)
		sigsuspend(&empty);

	return 0;
}

void signal_callback_handler(int signumber){
		printf("[Receiver] received SIGUSR1\n" );
		coutner++;
}


void final_callback_handler(int signumber){
		printf("[Receiver] I received: %d SIGUSR1 and now SIGUSR2 and I'm sending!\n", coutner);
		send_signals();

	exit(0);
}

void send_signals(){

	/* Check if parent is alive */
	pid_t pid = getppid();
	if((int)pid == 1){
		printf("Sory but my parent is dead, and I hope you wan't kill all your sesion :)!");
		return;
	}

	/*Start sending counter times to pid SIGUSR1 */
	while(coutner > 0){
		printf("[Receiver] sent SIGUSR1\n"); 
		kill(pid, SIGUSR1);

		coutner--;
	}

	/* Tell pid that you finished sending */
	kill(pid,SIGUSR2);
	
}


