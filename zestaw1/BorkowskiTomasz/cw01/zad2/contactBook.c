#include <stdio.h>
#include <stdlib.h>
#include "contact.h"
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>

#define elements 1000000
#define to_delete 10

void print_time_info(clock_t, struct tms *, struct tms *);
void generate_contacts(contact **, int);
void sort_test(contact **);
void find_and_delete_test(contact **, int);
void delete_test(contact **head);

int main(int argc, char const *argv[])
{	


	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error\n");

	contact * head = createBook();

   	printf("Generating %d and inserting contacts: \n", elements);
	printf("\tFun time: \n");
	generate_contacts(&head,elements);
	printf("\tMain time: \n" );

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);


   	printf("Sorting contacts:\n");
	printf("\tFun time: \n");
	sort_test(&head);
	printf("\tMain time: \n" );

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	printf("Finding and deleting %d random contacts: \n",to_delete);
	printf("\tFun time: \n");
	find_and_delete_test(&head, to_delete);
	printf("\tMain time: \n" );

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	printf("Cleaning book: \n");
	printf("\tFun time: \n");
	delete_test(&head);
	printf("\tMain time: \n" );

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

 
	return 0;
}


void generate_contacts(contact **head, int n)
{	
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");


	char name[100];
	char surname[100];
	char birthDate[100];
	char phone[100];	
	char address[100];
	
	strcpy(name,"Name  ");
	strcpy(surname,"Surname  ");
	strcpy(birthDate,"BirthDate  ");
	strcpy(phone,"Phone  ");
	strcpy(address,"address  ");


	for( int i = 0; i < n ; i++){
		int n = i%75 + 48;
		surname[strlen(surname)-1]=n;
		name[strlen(name)-1]=n;
		birthDate[strlen(birthDate)-1]=n;
		phone[strlen(phone)-1]=n;
		address[strlen(address)-1]=n;

		insertElement(head, createContact(surname,name,birthDate,phone,address));
	}

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);
}

void print_time_info(clock_t clock, struct tms *start_time, struct tms *current_time){

	long ticks = sysconf(_SC_CLK_TCK);;
	printf("\t\tReal time is:  \t%.2f\n", clock/ (double) ticks);
	printf("\t\tUser time is:  \t%.2f\n",( current_time->tms_utime - start_time->tms_utime )/(double) ticks );
	printf("\t\tSystem time is:\t%.2f\n",( current_time->tms_stime - start_time->tms_stime )/(double) ticks );

}


void sort_test(contact **head){
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");

   	sortBook(head);

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);
}


void find_and_delete_test(contact **head, int n){

	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");


   	// 
	srand(time(NULL));
   	
   	char text[10];
	strcpy(text,"Name A");
	contact *cur = NULL ;

	for(int i = 0 ; i < n ; i++ ){
		int n = rand()%75 + 48;
		text[strlen(text)-1]=n;
		
		if((cur = findElement(*head, text)) == NULL){
			printf("Sory, try again\n");
		} else {
			deleteElement(head,cur);
		}
		cur = NULL;
	}


   	//
	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

}

void delete_test(contact **head){
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");

   	deleteBook(head);

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);
}

