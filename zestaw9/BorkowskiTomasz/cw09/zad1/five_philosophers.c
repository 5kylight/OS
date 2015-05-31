#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>

#define PHILOSOPHERS_NUMBER 5

void *philosopher_thread(void *);

pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditions[PHILOSOPHERS_NUMBER];
int forks[PHILOSOPHERS_NUMBER];
	

int main(int argc, char *argv[])
{
	pthread_t philosophers[PHILOSOPHERS_NUMBER];

	for (int i = 0; i < PHILOSOPHERS_NUMBER; i++)
		pthread_create(&philosophers[i],  NULL, &philosopher_thread, (void *)(intptr_t) i);

	for (int i = 0; i < PHILOSOPHERS_NUMBER; i++)
		pthread_join(philosophers[i], NULL);
}


void *philosopher_thread(void * args)
{
	int position = (intptr_t) args;
	int counter = 0;

	int left = position;
	int right = (position + 1) % PHILOSOPHERS_NUMBER;

	while (1)
	{
		// sleep(1);
		pthread_mutex_lock(&monitor);
		
		/* Thinking */ 
		while(forks[left])
			pthread_cond_wait(&conditions[left], &monitor);
		while(forks[right])
			pthread_cond_wait(&conditions[right], &monitor);
		
		forks[right] = forks[left] = 1;

		printf("Philsopher nr %d \t get forks\n", position);

		pthread_mutex_unlock(&monitor);

		/* Eating */
		printf("Philsopher nr %d \t is eating\n", position);
		// sleep(2);
		counter++;

		forks[right] = forks[left] = 0;

		printf("Philsopher nr %d \t released forks\n", position);
		pthread_cond_signal(&conditions[left]);
		pthread_cond_signal(&conditions[right]);
	}
}