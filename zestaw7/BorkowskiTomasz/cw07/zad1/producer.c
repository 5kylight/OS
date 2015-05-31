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
#include <math.h>
#include <string.h>
#include <sys/time.h>
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

int 
main(int argc, char *arv[])
{	/*Prepare to exit*/
	atexit(before_exit);

	/*SIGINT signal to exit*/
	if (signal(SIGINT, sig_handler) < 0)
		error("signal");

	/*Create key for memory*/
	key_t shm_key = ftok(".", 1);

	/*Create key for semaphores*/
	key_t sem_key = ftok(".", 2);
	if (shm_key == -1 || sem_key == -1)
		error("ftok\n");

	/* Shared memory open */
	int shm_flags = IPC_CREAT | 0600;
	if ((shm_id = shmget(shm_key, SHM_SIZE, shm_flags)) == -1)
		error("shmget");
	

	/* Semaphore open */
	int sem_flags = IPC_CREAT | IPC_EXCL | 0600; 
	if ((sem_id = semget(sem_key, 2, sem_flags)) == -1)
	{
		if(errno == EEXIST)
			sem_exists = 1;
		else
			error("semget");
	}

	/*If this is first prodcucer create semaphores*/
	if (sem_exists)
	{
		if((sem_id = semget(sem_key, 2, 0)) == -1)
			error("semget");
	}
 	

	if ((memory = shmat(shm_id, NULL, 0)) == (void *) -1)
		error("shmat");

	/*Structure to set down empty semaphore*/
	struct sembuf SEM_EMPTY_DOWN;	
	SEM_EMPTY_DOWN.sem_flg = 0;
	SEM_EMPTY_DOWN.sem_num = SEM_EMPTY;
	SEM_EMPTY_DOWN.sem_op = -1;

	/*Structure to set up full semaphore*/
	struct sembuf SEM_FULL_UP;
	SEM_FULL_UP.sem_flg = 0;
	SEM_FULL_UP.sem_num = SEM_FULL;
	SEM_FULL_UP.sem_op = 1;
	
	/*Deal with shared memory*/
	first_task = (int *) memory;
	last_task = first_task + 1;
	task_tab = last_task + 1;

	union semun t;
	/*Initialize semaphores*/
	if(!sem_exists)
	{
		printf("First producer!\n");
		*first_task = 0;
		*last_task = 0;

		t.val = TASKS_LENGTH;
		if(semctl(sem_id, SEM_EMPTY, SETVAL, t) == -1)
			error("semctl sem producer");
		t.val = 0;
		if(semctl(sem_id, SEM_FULL, SETVAL, t) == -1)
			error("semctl sem consumer");

		/* Fill the memory of -1 */
		for(int i = 0; i < SHM_SIZE; i++)
			task_tab[i] = -1;
	}

	srand((unsigned int) time(NULL));

	while (1) 
	{	
		if(semop(sem_id, &SEM_EMPTY_DOWN, 1) == -1)
			error("semop down");
		
		insert_task();
		
		if(semop(sem_id, &SEM_FULL_UP, 1) == -1)
			error("semop up");
	}

	exit(EXIT_SUCCESS);
}

	/* Inserting number from 0 to MAX_NUMBER to array in shared memory*/
void
insert_task()
{	
	int num = rand() % MAX_NUMBER ;
	task_tab[*last_task] = num;

    char *current_time = calloc(16, sizeof(char));
    get_time(current_time);

    /*Printing info*/
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

	if(!sem_exists){
		shmctl(shm_id, IPC_RMID, NULL);
	}
	shmdt(memory);	

	/* if only one producer exit delete everything ! */	
	shmctl(shm_id, IPC_RMID, NULL);
	semctl(sem_id, 0, IPC_RMID, NULL);
	semctl(sem_id, 1, IPC_RMID, NULL);

}

void
error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}