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

int forking(int);
int vforking(int);
void cloning(int);
void vcloning(int);
void print_time_info(struct rusage parent, struct rusage child, struct timeval real_parent, struct timeval real_child, int n, char* method);
int process(); 
int counter = 0; 


int main(int argc, char *argv[]) { 


 	int N;
 	N = atoi(argv[1]);

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

}

int forking(int n)
{

	/* Prepare measure time! */
	struct timeval tmp, tmp1, real_parent, real_child; 


	real_child.tv_sec = 0;
	real_child.tv_usec = 0;
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
  	print_time_info(parent, child, real_parent, real_child, n, "fork");


  	return 0;
}	
int vforking(int n)
{
	/* Prepare measure time! */
	struct timeval tmp, tmp1, 
		real_parent, 
		real_child; 

	real_child.tv_sec = 0;
	real_child.tv_usec = 0;

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
  	print_time_info(parent, child, real_parent, real_child, n, "vfork");

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

	real_child.tv_usec = 0;
	real_child.tv_sec = 0;
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
  	print_time_info(parent, child, real_parent, real_child, n, "clone");

}

void vcloning(int n){

	/* Prepare measure time! */
	struct timeval tmp, tmp1, 
		real_parent, 
		real_child; 

	gettimeofday(&real_parent, NULL);
	real_child.tv_usec = 0;
	real_child.tv_sec = 0;

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
  	print_time_info(parent, child, real_parent, real_child, n, "vclone");

}



void print_time_info(struct rusage parent, struct rusage child, struct timeval real_parent, struct timeval real_child, int n, char *method){

	struct timeval sys_parent, usr_parent, sys_child, usr_child;	

    sys_parent = parent.ru_stime;
    usr_parent = parent.ru_utime;

  	/* Collecting times from childs */

    sys_child = child.ru_stime;
    usr_child = child.ru_utime;

  	char *tab[] = {"parent_sys", "parent_usr", "parent_real", "parent_sum",
  					"child_sys", "child_usr", "child_real", "child_sum",
  					"both_sys", "both_usr", "both_real", "both_sum"};

  	char *buff = (char*) malloc(sizeof(char)*(strlen(method)+30));
  	FILE *files[12];

  		for(int i = 0 ; i < 12; i++){
  			strcpy(buff,"./res/");
  			strcat(buff,method);
  			strcat(buff,"_");
  			strcat(buff,tab[i]);
  			files[i] = fopen(buff,"a");
  		}

  	float x;

  	printf("\t Parent sys time:     %f\n", 
        x = (float)(sys_parent.tv_sec + sys_parent.tv_usec/1000000.0));
  	fprintf(files[0], "%d %f\n", n, x);

  	printf("\t Parent usr time:     %f\n", 
        x = (float)(usr_parent.tv_sec + usr_parent.tv_usec/1000000.0));
  	fprintf(files[1], "%d %f\n", n, x);

  	printf("\t Parent real time:    %f\n",
        x =  (float)(real_parent.tv_sec + real_parent.tv_usec/1000000.0));
  	fprintf(files[2], "%d %f\n", n, x);

  	printf("\t Parent sys+usr       %f\n\n", 
        x = (float)(sys_parent.tv_sec + sys_parent.tv_usec/1000000.0) 
            + (float)(usr_parent.tv_sec + usr_parent.tv_usec/1000000.0));
  	fprintf(files[3], "%d %f\n", n, x);

  	/* Print sum */ 

  	printf("\t Childrens sys time:  %f\n",
         x = (float)(sys_child.tv_sec + sys_child.tv_usec/1000000.0) );
  	fprintf(files[4], "%d %f\n", n, x);
  	
  	printf("\t Childrens usr time:  %f\n",
        x = (float)(usr_child.tv_sec + usr_child.tv_usec/1000000.0) );
  	fprintf(files[5], "%d %f\n", n, x);
  	
  	printf("\t Childrens real time: %f\n", 
        x = (float)(real_child.tv_sec + real_child.tv_usec/1000000.0) );
  	fprintf(files[6], "%d %f\n", n, x);
	
    printf("\t Childrens sys+usr:   %f\n\n", 
        x = (float)(sys_child.tv_sec + sys_child.tv_usec/1000000.0) 
            + (float)(usr_child.tv_sec + usr_child.tv_usec/1000000.0));
    fprintf(files[7], "%d %f\n", n, x);


  	printf("\t Child+Par sys time:  %f\n", 
        x = (float)(sys_child.tv_sec + sys_child.tv_usec/1000000.0) 
            + (sys_parent.tv_sec + sys_parent.tv_usec/1000000.0)); 
  	fprintf(files[8], "%d %f\n", n, x);

  	printf("\t Child+Par usr time:  %f\n", 
        x = (float)(usr_child.tv_sec + usr_child.tv_usec/1000000.0) 
            + (usr_parent.tv_sec + usr_parent.tv_usec/1000000.0));
  	fprintf(files[9], "%d %f\n", n, x);

  	printf("\t Child+Par real time: %f\n", 
        x = (float)(real_child.tv_sec + real_child.tv_usec/1000000.0) 
            + (real_parent.tv_sec + real_parent.tv_usec/1000000.0));
  	fprintf(files[10], "%d %f\n", n, x);

    printf("\t Child+Par sys+usr:   %f\n", 
        x = (float)(sys_child.tv_sec + sys_child.tv_usec/1000000.0) 
            + (float)(usr_child.tv_sec + usr_child.tv_usec/1000000.0) 
            + (sys_parent.tv_sec + sys_parent.tv_usec/1000000.0) 
            + (float)(usr_parent.tv_sec + usr_parent.tv_usec/1000000.0));
  	fprintf(files[11], "%d %f\n", n, x);

  	  for(int i = 0 ; i < 12; i++){
  		 fclose(files[i]);
  	}
}


