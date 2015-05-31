#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

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

int 
main(int argc, char *arv[])
{
	atexit(before_exit);

	/*SIGINT to exit*/
	if (signal(SIGINT, sig_handler) < 0)
		error("signal");

	/*Geting memory and semaphores key*/
	key_t shm_key = ftok(".", 1);
	key_t sem_key = ftok(".", 2);
	if (shm_key == -1 || sem_key == -1)
		error("ftok\n");

	int shm_id;
	if ((shm_id = shmget(shm_key, SHM_SIZE, 0)) == -1)
		error("shmget");

	int sem_id;
	if ((sem_id = semget(sem_key, 2, 0)) == -1)
		error("semget");

	if ((memory = shmat(shm_id, NULL, 0)) == (void *) -1)
		error("shmat");

	/*Structure to set up empty semaphore*/
	struct sembuf SEM_EMPTY_UP;
	SEM_EMPTY_UP.sem_flg = 0;
	SEM_EMPTY_UP.sem_num = SEM_EMPTY;
	SEM_EMPTY_UP.sem_op = 1;

	/*Structure to set down full semaphore*/
	struct sembuf SEM_FULL_DOWN;
	SEM_FULL_DOWN.sem_flg = 0;
	SEM_FULL_DOWN.sem_num = SEM_FULL;
	SEM_FULL_DOWN.sem_op = -1;

	/*Deal with shared memory*/
	first_task = (int *) memory;
	last_task = first_task + 1;
	task_tab = last_task + 1;


	while (1) {
		if(semop(sem_id, &SEM_FULL_DOWN, 1) == -1)
			error("semop down");
		
		compute_task();
		
		if(semop(sem_id, &SEM_EMPTY_UP, 1) == -1)
			error("semop up");
	}

	exit(EXIT_SUCCESS);
}

/*Get number at index "first_task" in array and check if it is primary*/
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

	/*Printing info*/
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
	shmdt(memory);
}

void
error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}