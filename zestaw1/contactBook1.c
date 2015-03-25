#include <stdio.h>
#include <stdlib.h>
#include "contact.h"
void generateContacts(contact *);

int main(int argc, char const *argv[])
{

	contact * head =createBook();
	int  n = 5;
	contact * p;
	
	for( int i = 0 ; i < n ; i++)
	{
		p = (contact *) createContact("Tom","BO","21:15:14","13 4614 ","akr zpa kda h");
		// scanf("%s",p->surname); 
		insertElement(&head, p);
	}

	printf("\n" );
	// deleteBook(&head); // WORKS
	char * tab;
	tab = "E";
	p = findElement(head,tab); // WORKS
	// sortBook(&head);
	deleteElement(&head, p);
	print(head);
	printf("\n" );
	// printReverse(head);


// } 
	return 0;
}


void generateContacts(contact *head, int n)
{	
	char name[100];
	char surname[100];
	char birthDate[100];
	char phone[100];
	char address[100];
	

	name="Name";
	surname="surname";
	birthDate="birthDate";
	phone="phone";
	address="address";




	for( int i = 0; i < n ; i++){

		insertElement(&head, createContact(name,surname,birthDate,phone,address));
		strcat(name,i);
		strcat(surname,i);
		strcat(birthDate,i);
		strcat(phone,i);
		strcat(address,i);

	}
}