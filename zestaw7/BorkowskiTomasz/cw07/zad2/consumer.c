#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "config.h"

int is_prime(int);
void get_time(char *);
void compute_task();
void sig_handler(int);
void before_exit();
void error(char *);

int SHM_SIZE = 2 * sizeof(int) // index of first and last element of tasks
	+ TASKS_LENGTH * sizeof(int); // int array of tasks

void *memory;
int *first_task;
int *last_task;
int *task_tab;

sem_t *sem_full;
sem_t *sem_empty;

int 
main(int argc, char *argv[])
{
	/* Shared memory init */
	int shm_id;
	int shm_flags = O_RDWR | O_CREAT;
	if ((shm_id = shm_open("/memory", shm_flags, 0644)) == -1)
		error("shm_open");
	
	/* Trunc memory to size */
	if (ftruncate(shm_id, SHM_SIZE) == -1)
		error("ftruncate");

	memory = (void *) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_id, 0);
	if (memory == MAP_FAILED)
		error("mmap");

	/* Semaphores init */
	sem_empty = sem_open("/sem_empty", O_CREAT, 0644, 0);
	if(sem_empty == SEM_FAILED)
		error("sem_open empty");

	sem_full = sem_open("/sem_full", O_CREAT, 0644, 1);
	if(sem_full == SEM_FAILED)
		error("sem_open full");
	
	/*Deal with shared memory*/
	first_task = (int *) memory; 
	last_task = first_task + 1;
	task_tab = last_task + 1;

	while (1) 
	{
		sem_wait(sem_full);		

		compute_task();

		sem_post(sem_empty);
	}

	exit(EXIT_SUCCESS);
}

/*Check if int on index first_task is prime*/
void
compute_task()
{

	int num = task_tab[*first_task];

	char *info;
	if(is_prime(num))
		info = "    ";
	else
		info = "not ";
	
    char *current_time = calloc(16, sizeof(char));
	get_time(current_time);

	/*Print info*/
	printf("(%d %s) I checked number %d\t- is %sprime. Task left: %d\n", 
		getpid(),
		current_time,
		num,
		info,
		*last_task >= *first_task ? *last_task - *first_task : TASKS_LENGTH - *first_task + *last_task);

	*first_task = (*first_task + 1) % TASKS_LENGTH;

	free(current_time);
}

/* Returns 1 if is prime else return 0 */
int
is_prime(int n)
{	
	if(n <= 1)
		return 0;

	int i = 2; 
	while (i*i <= n)
	{	
		if (!(n%i++))
			return 0;
	}
	return 1;
}

/* Fun to get time in format: HH:MM:SS:NNNNNN
where NNNNNN are miliseconds 
*/ 
void 
get_time(char *hours)
{
    struct timeval mytime;
    gettimeofday(&mytime, NULL);
    char *res = (char *) ctime(&(mytime.tv_sec));
    strncpy(hours, res+11, 8);
    char *mili = calloc(7, sizeof(char));
    sprintf(mili, "%d", (int) mytime.tv_usec/1000 );
    strcat(hours, ":");
    strcat(hours, mili);
    free(mili);
}


void
sig_handler(int signum)
{
	printf("\nExit\n");
	exit(EXIT_SUCCESS);
}

void
before_exit()
{
	sem_close(sem_full);
	sem_close(sem_empty);
}

void
error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}