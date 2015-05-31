#define _BSD_SOURCE 
#define _POSIX_SOURCE
#define _XOPEN_SOURCE 500

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define RECORD_SIZE 1024

int find(char *word);
void before_exit();
void error(char *);
void *search(void *look_for);
void cancelAll(unsigned long);
void cleanup_handler(void *arg);
void usage(char *);
void print_thread_info(int);

/*Only to tests*/
void kill_my_process();

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int file;
int records_number;
int threads_number;
pthread_t *threads;
int not_done = 1;

int version;
int sig_number;

int main(int argc, char *argv[])
{	

	/*Prepare to exit*/
	atexit(before_exit);

	if(argc != 7)
	{
		usage(argv[0]);
	
	}
	
	/*Zad 2*/
	version = atoi(argv[5]);
	sig_number = atoi(argv[6]);

	/* Zad1 */
	threads_number = atoi(argv[1]);
	char *file_name = argv[2];
	records_number = atoi(argv[3]);
	char *word = argv[4];
	

	/*Opening file*/	
	if((file = open(file_name, O_RDONLY)) < 0)
		error("open file");

	threads = (pthread_t *) calloc(threads_number, sizeof(pthread_t));				// Remember to free it!
	for(int i = 0; i < threads_number; i++)
	{
		pthread_create(&threads[i], NULL, &search, word);
	}

	void *status;

	not_done = 0;
	sigset_t mask_set;

	sleep(1);

	switch(version)
	{
		case 1:
			printf("Version 1\n");
			kill_my_process(sig_number);
		break;

		case 2:
			printf("Version 2\n");

			sigemptyset(&mask_set);
			sigaddset(&mask_set, sig_number);
			sigprocmask(SIG_BLOCK, &mask_set, NULL);
			kill_my_process(sig_number);
		break;

		case 3:
			printf("Version 3\n");
			kill_my_process(sig_number);
		break;

		case 4:
			printf("Version 4\n");

		case 5:
			if(version == 5)
				printf("Version 5\n");

			for(int i = 0; i < threads_number; i++)
			{
				pthread_kill(threads[i], sig_number);
			}
		break;
	}
	
	for(int i = 0; i < threads_number; i++)
	{
		pthread_join(threads[i], &status);
	}

	return 0;
}	


void *search(void *look_for)
{

	/*Zad 2*/
	struct sigaction sig_act;
	sigset_t mask_set;

	switch(version)
	{
		case 3:
			printf("Version 3 in thread\n");
		case 5:
			if(version == 5)
				printf("Version 5 in thread\n");

			sig_act.sa_handler = print_thread_info;
			sig_act.sa_flags = 0;

			if(sigaction(sig_number, &sig_act, NULL) == -1)
				error("sigaction");
		break;

		case 4:
			printf("Version 4 in thread\n");

			sigemptyset(&mask_set);
			sigaddset(&mask_set, sig_number);
			pthread_sigmask(SIG_BLOCK, &mask_set, NULL);
		break;
	}

	/* Zad1 */
	int bytes_read = RECORD_SIZE;
	char *word = (char *) look_for;

	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0)
		error("pthread_setcanceltype");


	while(not_done);
	sleep(3);

	char text[records_number][RECORD_SIZE + 1];

	while(bytes_read == RECORD_SIZE)
	{	
		int read_records = 0;
		
		pthread_cleanup_push(cleanup_handler, NULL);
		if (pthread_mutex_lock(&mutex) != 0)
			error("Mutex lock");
		
		while(read_records < records_number)
		{
			bytes_read = read(file, text[read_records], RECORD_SIZE);
			
			if(bytes_read != RECORD_SIZE)
				break;

			text[read_records][RECORD_SIZE] = 0;
			read_records++;
		}

		if (pthread_mutex_unlock(&mutex) != 0)
			error("Mutex unlock");
		pthread_cleanup_pop(0);


		char *save_ptr;
		char *id;
		for(int i = 0; i < read_records; i++)
		{
			id = strtok_r(text[i], " ", &save_ptr);
			if(strstr(save_ptr, word) != NULL)
			{
				pthread_t myID = (unsigned long) pthread_self();
				printf("%ld I found that in record number in %s\n", myID, id);
			}
		}
	}

	return NULL;

}

void error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void before_exit()
{
	free(threads);
}

void cancelAll(unsigned long myID)
{
	for (int i = 0; i < threads_number; i++)
	{
		if ((unsigned long) threads[i] == myID)
			continue;
		if (pthread_cancel(threads[i]) < 0)
			error("cancel");
	}
}

void cleanup_handler(void *arg)
{
	if (pthread_mutex_unlock(&mutex) != 0)
		error("Mutex unlock");
}
/* Zad 1 ends */
/* Zad 2 */

void kill_my_process(int signo)
{
	kill(getpid(), signo);
}

void usage(char *prog_name)
{
	printf("Bad arguments!\n");
	printf("Usage: \n");
	printf("<%s> <number of threads> <file_name> <number of records> <word> <version> <signal>\n", prog_name);
	printf("Signals:\n");
	printf("\t9  - SIGKILL\n");
	printf("\t15 - SIGTERM\n");
	printf("\t17 - SIGSTOP\n");
	printf("\t10 - SIGUSR1\n");	
	printf("Versions:\n");
	printf("\t1 - Send signals to all threads with no service definied for this singal\n");
	printf("\t2 - Send signals to only main has definied sevice, but other threads no\n");
	printf("\t3 - Send signals to process and all threads definied sevice and they print PID TID\n");
	printf("\t4 - Send signal to thread with definied sevice\n");
	printf("\t5 - Send signal to thread with definied sevice with fun that print info\n");

	printf("\t\n" );
	exit(EXIT_FAILURE);
}

void print_thread_info(int signo)
{
	printf("Greetings from thread  I recived %d my info: \n", signo);
	printf("\tTID: %ld \n", (unsigned long) pthread_self());
	printf("\tPID: %d \n", getpid());
}
