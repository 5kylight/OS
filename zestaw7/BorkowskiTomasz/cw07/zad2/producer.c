#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "config.h"

#define MAX_NUMBER 1000000

int is_partial(int);
void get_time(char *);
void insert_task();
void sig_handler(int signum);
void before_exit();
void error(char *);

int SHM_SIZE = 2 * sizeof(int) // index of first and last element of tasks
	+ TASKS_LENGTH * sizeof(int); // int array of tasks

void *memory;
int *first_task;
int *last_task;
int *task_tab;
int sem_exists = 0;

int shm_id;
int sem_id;

sem_t *sem_full;
sem_t *sem_empty;

int 
main(int argc, char *argv[])
{
	atexit(before_exit);

	if (signal(SIGINT, sig_handler) < 0)
		error("signal");

	/* Shared memory open */
	int shm_flags = O_RDWR | O_CREAT;
	if ((shm_id = shm_open("/memory", shm_flags, 0644)) == -1)
		error("shm_open");
	
	/* Trunc memory to size */
	if (ftruncate(shm_id, SHM_SIZE) == -1)
		error("ftruncate");

	memory = (void *) mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_id, 0);
	if (memory == MAP_FAILED)
		error("mmap");

	/* Semaphores open */
	sem_empty = sem_open("/sem_empty", O_CREAT, 0644, TASKS_LENGTH);
	if(sem_empty == SEM_FAILED)
		error("sem_open empty");

	sem_full = sem_open("/sem_full", O_CREAT, 0644, 1);
	if(sem_full == SEM_FAILED)
		error("sem_open full");

	/*Deal with shared memory*/
	first_task = (int *) memory;	
	last_task = first_task + 1;
	task_tab = last_task + 1;

	srand((unsigned int) time(NULL));

	while (1) 
	{	
		sem_wait(sem_empty);
		
		insert_task();
		
		sem_post(sem_full);
	}
	exit(EXIT_SUCCESS);
}

/*Inserts number at index last_task*/
void
insert_task()
{	
	int num = rand() % MAX_NUMBER ;
	
	task_tab[*last_task] = num;

    char *current_time = calloc(16, sizeof(char));
    get_time(current_time);

    /*Print info*/
	printf("(%d %s) I inserted number %d.\t Task left: %d\n",
		getpid(),
		current_time,
		num,
		*last_task >= *first_task ? *last_task - *first_task : TASKS_LENGTH - *first_task + *last_task);

	(*last_task) = (*last_task + 1) % TASKS_LENGTH;
	
	free(current_time);

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
	exit(EXIT_SUCCESS);
}

void
before_exit()
{
	munmap(memory, SHM_SIZE);
	sem_close(sem_full);
	sem_close(sem_empty);
	shm_unlink("/memory");
	sem_unlink("/sem_empty");
	sem_unlink("/sem_full");
}

void
error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}