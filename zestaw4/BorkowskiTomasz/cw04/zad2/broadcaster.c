#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>   
	
void signal_callback_handler(int);
void final_callback_handler(int);

int coutner = 0;
pid_t pid;

int main(int argc, char *argv[]){

	/* Prepare to receive signals */
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

	/* Signals to send */ 
	if(argc != 2){
		perror("Invalid argument number!");
		return 1;
	}
	int signals_to_send = atoi(argv[1]);

	sigset_t old_set, new_set;
	sigaddset(&new_set, SIGUSR1);
	sigaddset(&new_set, SIGUSR2);

	sigprocmask(SIG_BLOCK, &new_set, &old_set);

	if((pid = fork()) >= 0){
		if(pid == 0){
			execl("./receiver", "receiver", (char*)NULL);
		} 
	} else {
		printf("Forking error\n");
	}


	/*Start sending counter times to pid SIGUSR1 */
	while(signals_to_send > 0){
		printf("[Broadcaster] sent SIGUSR1\n"); 
		kill(pid, SIGUSR1);
		sigsuspend(&old_set);

		signals_to_send--;
	}
	
	/* Tell pid that you finished sending */
	kill(pid,SIGUSR2);

	while(1)
		sigsuspend(&old_set);

	return 0;
}

void signal_callback_handler(int signumber){
	printf("[Broadcaster] received SIGUSR2\n" );
	coutner++;
}


void final_callback_handler(int signumber){
	printf("[Broadcaster] I received %d SIGUSR1!\n", coutner);

	exit(0);
}



