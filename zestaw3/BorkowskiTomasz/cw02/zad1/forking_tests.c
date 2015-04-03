#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <linux/sched.h>
#include <sys/time.h>	
#include <sched.h>
#include <sys/resource.h>
#define STACK_SIZE 1000
#define N 10000

int forking(int);
int vforking(int);
void cloning(int);
void vcloning(int);
void print_time_info(struct rusage parent, struct rusage child, struct timeval real_parent, struct timeval real_child);
int process(); 
int counter = 0; 


int main(int argc, char *argv[]) { /* Prepare to measure time */
	/* Prepare measure time! */
	struct timeval tmp1, 
		real_parent, 
		real_child; 

	gettimeofday(&real_parent, NULL);

	/* Time started */
 		
	 #ifdef FORK   	
		 /* Forking tests */
		forking(N);
	#elif VFORK
		/* Vforking tests */
		vforking(N);
   	#elif CLONE	
		/* Cloning as forking test */
		cloning(N);
	#elif VCLONE
		/* Cloning as vforking test */
		vcloning(N);
	#endif		
  	/* Setting up finish time */
  	gettimeofday(&tmp1,NULL);
  	real_parent.tv_sec = tmp1.tv_sec - real_parent.tv_sec;
  	real_parent.tv_usec = tmp1.tv_usec - real_parent.tv_usec;


  	/* Collection information for parent process */ 
  	struct rusage parent, child;
  	getrusage(RUSAGE_SELF, &parent);
  	getrusage(RUSAGE_CHILDREN, &child);


  	/* Printing info */

   	printf("[Main] took: \n");
  	print_time_info(parent, child, real_parent, real_child);


}

int forking(int n)
{

	/* Prepare measure time! */
	struct timeval tmp, tmp1, 
		real_parent, 
		real_child; 

	gettimeofday(&real_parent, NULL);

	/* Time started */


  	int status;	
  	pid_t pid;
  	for(int i = 0; i < n ; i++){
			gettimeofday(&tmp, NULL);
  		if((pid = fork()) < 0)
  			printf("Forking error!\n");
  		else if(pid == 0){
  			counter++;
  			_exit(0);
  		} else {
  			waitpid(pid, &status, 0);
			gettimeofday(&tmp1,	NULL);
  			real_child.tv_sec += tmp1.tv_sec - tmp.tv_sec;
  			real_child.tv_usec += tmp1.tv_usec - tmp.tv_usec;
  		}
  	}

  	/* Setting up finish time */
  	gettimeofday(&tmp1,NULL);
  	real_parent.tv_sec = tmp1.tv_sec - real_parent.tv_sec;
  	real_parent.tv_usec = tmp1.tv_usec - real_parent.tv_usec;


  	/* Collection information for parent process */ 
  	struct rusage parent, child;
  	getrusage(RUSAGE_SELF, &parent);
  	getrusage(RUSAGE_CHILDREN, &child);


  	/* Printing info */

  	printf("[Forking fun] Counter = %d\n", counter);
  	print_time_info(parent, child, real_parent, real_child);


  	return 0;
}	
int vforking(int n)
{
	/* Prepare measure time! */
	struct timeval tmp, tmp1, 
		real_parent, 
		real_child; 

	gettimeofday(&real_parent, NULL);

	/* Time started */

  	int status;	
   	pid_t pid;
   	for(int i = 0; i < n ; i++){

   		gettimeofday(&tmp, NULL);
   		if((pid = vfork()) < 0)
   			printf("vForking error!\n");
   		else if(pid == 0){
   			counter++;
   			_exit(0);
   		} else {
   			waitpid(pid, &status, 0);
   			gettimeofday(&tmp1,	NULL);
  			real_child.tv_sec += tmp1.tv_sec - tmp.tv_sec;
  			real_child.tv_usec += tmp1.tv_usec - tmp.tv_usec;
   		}

   	}

  	/* Printing info */
  	gettimeofday(&tmp1,NULL);
  	real_parent.tv_sec = tmp1.tv_sec - real_parent.tv_sec;
  	real_parent.tv_usec = tmp1.tv_usec - real_parent.tv_usec;


  	/* Collection information for parent process */ 
  	struct rusage parent, child;
  	getrusage(RUSAGE_SELF, &parent);
  	getrusage(RUSAGE_CHILDREN, &child);


  	/* Printing info */

   	printf("[Vforking fun] Counter = %d\n",counter);
  	print_time_info(parent, child, real_parent, real_child);

   	return 0;
}	

int process(){
	counter++;
	_exit(0);
}

void cloning(int n){
	/* Prepare measure time! */
	struct timeval tmp, tmp1, 
		real_parent, 
		real_child; 

	gettimeofday(&real_parent, NULL);

	/* Time started */






   char *stack;
   stack =(char*) malloc(STACK_SIZE);
   stack += STACK_SIZE;
   int status;
   for(int i = 0; i < n ; i++){
  	 	gettimeofday(&tmp, NULL);
    	clone(&process,stack ,SIGCHLD, (char*) NULL );
    	wait(&status);
    	gettimeofday(&tmp1,	NULL);
  		real_child.tv_sec += tmp1.tv_sec - tmp.tv_sec;
  		real_child.tv_usec += tmp1.tv_usec - tmp.tv_usec;
   }
   
  	/* Printing info */
  	gettimeofday(&tmp1,NULL);
  	real_parent.tv_sec = tmp1.tv_sec - real_parent.tv_sec;
  	real_parent.tv_usec = tmp1.tv_usec - real_parent.tv_usec;


  	/* Collection information for parent process */ 
  	struct rusage parent, child;
  	getrusage(RUSAGE_SELF, &parent);
  	getrusage(RUSAGE_CHILDREN, &child);


  	/* Printing info */

   	printf("[Cloning fun] Counter = %d\n",counter);
  	print_time_info(parent, child, real_parent, real_child);

}

void vcloning(int n){
	/* Prepare measure time! */
	struct timeval tmp, tmp1, 
		real_parent, 
		real_child; 

	gettimeofday(&real_parent, NULL);

	/* Time started */

   int status;
   char *stack;
   stack =(char*) malloc(STACK_SIZE);
   stack += STACK_SIZE;
   for(int i = 0; i < n ; i++){
   		gettimeofday(&tmp, NULL);
	   	clone(&process,stack , SIGCHLD | CLONE_VM | CLONE_VFORK, (char*) NULL );
   		wait(&status);
   		gettimeofday(&tmp1,	NULL);
  		real_child.tv_sec += tmp1.tv_sec - tmp.tv_sec;
  		real_child.tv_usec += tmp1.tv_usec - tmp.tv_usec;
   }
   
	  	/* Printing info */
  	gettimeofday(&tmp1,NULL);
  	real_parent.tv_sec = tmp1.tv_sec - real_parent.tv_sec;
  	real_parent.tv_usec = tmp1.tv_usec - real_parent.tv_usec;


  	/* Collection information for parent process */ 
  	struct rusage parent, child;
  	getrusage(RUSAGE_SELF, &parent);
  	getrusage(RUSAGE_CHILDREN, &child);


  	/* Printing info */

   	printf("[VCloning fun] Counter = %d\n",counter);
  	print_time_info(parent, child, real_parent, real_child);

}



void print_time_info(struct rusage parent, struct rusage child, struct timeval real_parent, struct timeval real_child){

	struct timeval sys_parent, usr_parent, sys_child, usr_child;	
	sys_parent.tv_sec = parent.ru_stime.tv_sec;
  	sys_parent.tv_usec = parent.ru_stime.tv_usec;
  	usr_parent.tv_sec = parent.ru_utime.tv_sec;
  	usr_parent.tv_usec = parent.ru_utime.tv_usec;

  	/* Collecting times from childs */

  	sys_child.tv_sec = child.ru_stime.tv_sec;
  	sys_child.tv_usec = child.ru_stime.tv_usec;
  	usr_child.tv_sec = child.ru_utime.tv_sec;
  	usr_child.tv_usec = child.ru_utime.tv_usec;


  	printf("\t Parent sys time:     %f\n",(float)(sys_parent.tv_sec + sys_parent.tv_usec/1000000.0) );
  	printf("\t Parent usr time:     %f\n",(float)(usr_parent.tv_sec + usr_parent.tv_usec/1000000.0) );
  	printf("\t Parent real time:    %f\n",(float)(real_parent.tv_sec + real_parent.tv_usec/1000000.0) );

  	printf("\t Childrens sys time:  %f\n",(float)(sys_child.tv_sec + sys_parent.tv_usec/1000000.0) );
  	printf("\t Childrens usr time:  %f\n",(float)(usr_child.tv_sec + usr_parent.tv_usec/1000000.0) );
  	printf("\t Childrens real time: %f\n",(float)(real_child.tv_sec + real_parent.tv_usec/1000000.0) );
}


