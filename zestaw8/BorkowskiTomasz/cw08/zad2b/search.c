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

	if(argc != 5)
	{
		usage(argv[0]);
	
	}
	
	/* Arguments */
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
	sleep(1);
	
	for(int i = 0; i < threads_number; i++)
	{
		pthread_join(threads[i], &status);
	}

	printf("Sorry, try againg\n");
	return 0;
}	


void *search(void *look_for)
{

	int bytes_read = RECORD_SIZE;
	char *word = (char *) look_for;

	if(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0)
		error("pthread_setcanceltype");

	while(not_done);
	sleep(1);

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

		        if(rand() > RAND_MAX - RAND_MAX/10){
					int x = 5/0;
					printf("Ha ha ha I can divide by 0%d\n", x);
		        }

			}
		}
		 srand(time(NULL));
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

void kill_my_process(int signo)
{
	kill(getpid(), signo);
}

void cleanup_handler(void *arg)
{
	if (pthread_mutex_unlock(&mutex) != 0)
		error("Mutex unlock");
}

void usage(char *prog_name)
{
	printf("Bad arguments!\n");
	printf("Usage: \n");
	printf("<%s> <number of threads> <file_name> <number of records> <word>\n", prog_name);
	exit(EXIT_FAILURE);
}