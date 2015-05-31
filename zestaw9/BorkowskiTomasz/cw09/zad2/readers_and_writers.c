#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define READERS 4
#define WRITERS 5

void error(char *);
void *reader_thread(void *args);
void *writer_thread(void *args);

int buffer[BUFFER_SIZE];

sem_t sem_free;
sem_t sem_writers_block;

int active_readers = 0;
int working_readers = 0;

int active_writers = 0;
int working_writers = 0;

int main(int argc, char *argv[])
{
	/* Initializing semaphores */
	if(sem_init(&sem_free, 0, READERS) || sem_init(&sem_writers_block, 0, 1))
		error("sem_init main semaphores");

	srand(time(NULL));

	pthread_t readers[READERS];
	pthread_t writers[WRITERS];

	int n = 5;

	for (int i = 0; i < WRITERS; i++)
		pthread_create(&writers[i],  NULL, &writer_thread, NULL);
	
	for (int i = 0; i < READERS; i++)
		pthread_create(&readers[i],  NULL, &reader_thread, (void *)(intptr_t) n);
	
	for (int i = 0; i < READERS; i++)
		pthread_join(readers[i], NULL);

	for (int i = 0; i < WRITERS; i++)
		pthread_join(writers[i], NULL); 

}

void *reader_thread(void *args)
{
	int n = (intptr_t) args;
	int counter;
	while (1)
	{
		/* Prepare to read */

		sem_wait(&sem_free);

		/* Reading */
		counter = 0;
		for(int i = 0; i < BUFFER_SIZE; i++)
			if(buffer[i] == n)
			{
				// printf("[Reader] I found %d\t at %d\n", n, i);
				counter++;
			}

		if(counter > 0)
			printf("[Reader] I found\t %d existings of %d\n", counter, n);
		
		/* Cleanup reading */
		sem_post(&sem_free);
		
	}
}

void *writer_thread(void *args)
{
	int insertions; 
	int index;
	int n;
	while (1)
	{
		/* Prepare to write*/
		sem_wait(&sem_writers_block);
		for (int i = 1; i < READERS; i++)
			sem_wait(&sem_free);

		/* Writting */
		insertions = rand();
		for (int i = 0; i < insertions; i++)
		{
			n = buffer[index = rand()%BUFFER_SIZE] = rand()%10;
			printf("[Writer] I inserted\t %d on index %d\n", n, index);
		}

		/* Cleanup after writing */
		for (int i = 1; i < READERS; i++)
			sem_post(&sem_free);

		sem_post(&sem_writers_block);

	}
}

void error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
