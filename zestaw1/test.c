#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
typedef struct contact{
	struct contact * next;
	struct contact * prev;
	char name[15];
	// char surname[20];
	// char birthDate[8];
	// char phone[10];
	// char address[20];
} contact;

void deletee(contact ** head){

	free(*head);
}

void print(contact ** head){
	strcpy((*head)->name,"cos");
	printf("%s\n", (*head)-> name );	
}

int main(){
	char t1[15];
	char t2[10];
	strcpy(t1,"ala   ");
	strcpy(t2,"ala     ");
	srand(time(NULL));

	printf("%d\n", rand()%47);
	// contact * con = NULL;
	//  // = (contact * ) malloc(sizeof(contact));
	
	// if( con -> next == NULL)
	// 	printf("siama ias am\n" );
	// deletee(&con);
	// int n =3565431;
	// contact * tab[n];
	// 	for(int i = 0 ; i < n ; i++){
	// 		tab[n] = (contact * ) calloc(1,sizeof(contact)); 
	// 		strcpy(tab[n]->name, "ala ma kota");
	// 		free(tab[n]);
	// 	}
	 // contact *con1 = (contact * ) calloc(1,sizeof(contact));
	 // contact *con5 = (contact * ) calloc(1,sizeof(contact));
	 // contact *con2= (contact * ) calloc(1,sizeof(contact));
	 // contact *con3 = (contact * ) calloc(1,sizeof(contact));
	 // contact *con4 = (contact * ) calloc(1,sizeof(contact));
	 // free(&con);
	// print(&con);

}