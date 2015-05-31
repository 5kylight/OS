#define _BSD_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define RECORD_SIZE 1024

int find(char *word);
void before_exit();
void error(char *);
void *search(void *look_for);
void cancelAll(unsigned long);
void cleanup_handler(void *arg);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int file;
int records_number;
int threads_number;
pthread_t *threads;
int not_done = 1;

int main(int argc, char *argv[])
{	
	/*Prepare to exit*/
	atexit(before_exit);

	if(argc != 5)
	{
		printf("Bad arguments! Usage: <%s> <number of threads> <file_name> <number of records> <word>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Arguments */
	threads_number = atoi(argv[1]);
	char *file_name = argv[2];
	records_number = atoi(argv[3]);
	char *word = argv[4];
	

	/*Opening file*/	
	if((file = open(file_name, O_RDONLY)) < 0)
		error("open file");

	/* Creating threads */
	threads = (pthread_t *) calloc(threads_number, sizeof(pthread_t));				// Remember to free it!
	for(int i = 0; i < threads_number; i++)
	{
		pthread_create(&threads[i], NULL, &search, word);
	}

	/* Signal threads that all threads are created */
	not_done = 0;

	void *status;

	/* Joining threads*/
	for(int i = 0; i < threads_number; i++)
	{
		pthread_join(threads[i], &status);
	}

	return 0;
}	


void *search(void *look_for)
{
	int bytes_read = RECORD_SIZE;
	char *word = (char *) look_for;

	while(not_done);

	char text[records_number][RECORD_SIZE + 1];

	while(bytes_read == RECORD_SIZE)
	{	
		int read_records = 0;
		
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
		
        pthread_testcancel();

		char *save_ptr;
		char *id;
		for(int i = 0; i < read_records; i++)
		{
			id = strtok_r(text[i], " ", &save_ptr);
			if(strstr(save_ptr, word) != NULL)
			{
				pthread_t myID = (unsigned long) pthread_self();
				printf("%ld I found that in record number in %s\n", myID, id);
				cancelAll(myID);
			}
		}
	}
	return NULL;
}

void error(char *msg)
{
	perror("msg");
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