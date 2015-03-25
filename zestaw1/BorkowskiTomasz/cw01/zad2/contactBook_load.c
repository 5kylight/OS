#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include <dlfcn.h>

#define elements 1000000
#define to_delete 10

typedef struct contact{
	struct contact *next;
	struct contact *prev;
	char name[15];
	char surname[20];
	char birthDate[8];
	char phone[10];
	char address[20];
} contact;


typedef contact *(*createBook_fun)();
typedef contact *(*createContact_fun)(char *, char *, char *,char *, char *);
typedef void (*deleteBook_fun)(struct contact **);
typedef void (*insertElement_fun)(struct contact **, struct contact *);
typedef void (*deleteElement_fun)(struct contact **, struct contact *);
typedef struct contact *(*findElement_fun)(struct contact *, char *);
typedef void (*sortBook_fun)(struct contact **);
typedef void (*print_fun)(struct contact *);

void print_time_info(clock_t, struct tms *, struct tms *);
void generate_contacts(struct contact **, int, createContact_fun, insertElement_fun);
void sort_test(struct contact **, sortBook_fun );
void find_and_delete_test(struct contact **, int,  findElement_fun, deleteElement_fun);
void delete_test(struct contact **head, deleteBook_fun );



int main(int argc, char const *argv[])
{	

	void *handle;
	handle = dlopen("../zad1/libcontact.so",RTLD_LAZY);
	if(!handle){
		fputs (dlerror(), stderr);
	    exit(1);
	}


	createBook_fun createBook = ( createBook_fun ) dlsym(handle,"createBook");
	createContact_fun createContact = (createContact_fun) dlsym(handle,"createContact");
	insertElement_fun insertElement = (insertElement_fun)dlsym(handle,"insertElement");
	deleteBook_fun deleteBook = (deleteBook_fun) dlsym(handle,"deleteBook");
	deleteElement_fun deleteElement = (deleteElement_fun) dlsym(handle,"deleteElement");
	sortBook_fun sortBook = (sortBook_fun) dlsym(handle,"sortBook");
	findElement_fun findElement = (findElement_fun) dlsym(handle,"findElement");

	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error\n");

	contact *head=NULL;
	head = createBook();

   	printf("Generating %d and inserting contacts: \n", elements);
		printf("\tFun time: \n");
		generate_contacts(&head,elements,createContact,insertElement);				//executing
		printf("\tMain time: \n" );
		if((stop_time = times(&stop_tms)) == -1)
	   		printf("Time error\n");
	   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	printf("Sorting contacts:\n");

		printf("\tFun time: \n");
		sort_test(&head, sortBook);
		printf("\tMain time: \n" );

		if((stop_time = times(&stop_tms)) == -1)
	   		printf("Time error\n");
	   	
	   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	printf("Finding and deleting %d random contacts: \n",to_delete);
		printf("\tFun time: \n");
		find_and_delete_test(&head, to_delete, findElement, deleteElement);
		printf("\tMain time: \n" );

		if((stop_time = times(&stop_tms)) == -1)
	   		printf("Time error\n");
	   	
	   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

   	printf("Cleaning book: \n");
		printf("\tFun time: \n");
		delete_test(&head, deleteBook);
		printf("\tMain time: \n" );

		if((stop_time = times(&stop_tms)) == -1)
	   		printf("Time error\n");
	   	
	   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

	 dlclose(handle);
	 
	return 0;
}


void generate_contacts(struct contact **head, int n,
	createContact_fun createContact,
	insertElement_fun insertElement)
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
  


void print_time_info(clock_t clock, struct tms *start_time, struct tms *current_time)
{

	long ticks = sysconf(_SC_CLK_TCK);;
	printf("\t\tReal time is:  \t%.2f\n", clock/ (double) ticks);
	printf("\t\tUser time is:  \t%.2f\n",( current_time->tms_utime - start_time->tms_utime )/(double) ticks );
	printf("\t\tSystem time is:\t%.2f\n",( current_time->tms_stime - start_time->tms_stime )/(double) ticks );

}


void sort_test(struct contact **head, sortBook_fun sortBook){
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");

   	sortBook(head);

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);
}


void find_and_delete_test(struct contact **head, int n, findElement_fun findElement,deleteElement_fun deleteElement)
{
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");

	srand(time(NULL));

   	char text[10];
	strcpy(text,"Name A");
	struct contact *cur = NULL ;

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


	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

}

void delete_test(struct contact **head, deleteBook_fun deleteBook){
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;

   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");

   	deleteBook(head);

	if((stop_time = times(&stop_tms)) == -1)
   		printf("Time error\n");
   	
   	print_time_info(stop_time - start_time, &start_tms, &stop_tms);
}

