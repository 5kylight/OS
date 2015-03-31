#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>

#define _BSD_SOURCE 
int forking(int);
int vforking(int);
int cloning(void);
int vcloning(void);
void print_time_info(clock_t, struct tms *, struct tms *);

int counter = 0;
int main(int argc, char *argv[])
{
	/* Prepare to measure time */
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   
   /* Start measure time */ 
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error\n");

   	/* Time started */
   	
	forking(10000);


	vforking(10000);
   	/* Stop measure time */
	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	/* Printing time info */
   	printf("[Main] took: \n");
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

}

int forking(int n)
{
	/* Prepare to measure time */
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   
   /* Start measure time */ 
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error\n");

   	/* Time started */

  	int status;	
   	pid_t pid;
   	for(int i = 0; i < n ; i++){
   		if((pid = fork()) < 0)
   			printf("Forking error!\n");
   		else if(pid == 0){
   			counter++;
   			_exit(0);
   		} else {
   			waitpid(pid, &status, 0);
   		}

   	}
   	printf("[Forking fun] Counter = %d\n",counter);

   	/* Stop measure time */
	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	/* Printing time info */
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	return 0;
}	
int vforking(int n)
{
	/* Prepare to measure time */
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   
   /* Start measure time */ 
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error\n");

   	/* Time started */

  	int status;	
   	pid_t pid;
   	for(int i = 0; i < n ; i++){
   		if((pid = vfork()) < 0)
   			printf("vForking error!\n");
   		else if(pid == 0){
   			counter++;
   			_exit(0);
   		} else {
   			waitpid(pid, &status, 0);
   		}

   	}
   	printf("[Vforking fun] Counter = %d\n",counter);

   	/* Stop measure time */
	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	/* Printing time info */
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	return 0;
}	

void print_time_info(clock_t clock, struct tms *start_time, struct tms *current_time){

	long ticks = sysconf(_SC_CLK_TCK);

	double user, sys;
	printf("\t\tReal time is:  				\t%.2f\n", clock/ (double) ticks);
	printf("\t\tUser time is: 				\t%.2f\n",(user = ( current_time->tms_utime - start_time->tms_utime )/(double) ticks) );
	printf("\t\tSystem time is:				\t%.2f\n",(sys = ( current_time->tms_stime - start_time->tms_stime )/(double) ticks));
	printf("\t\tUser + System time time is:	\t\t%.2f\n",user+sys );
}