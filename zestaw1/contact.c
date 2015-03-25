#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "contact.h"


// Creating book
contact *createBook()
{

	return (contact*) malloc(sizeof(contact*));
}
// Creating element
contact *createContact(char *name, char *surname, char *birthDate, char *phone, char *address)
{
	contact *element = (contact*)  malloc(sizeof(contact));
	strcpy(element->name, name);
	strcpy(element->surname,surname);
	strcpy(element->birthDate, birthDate);
	strcpy(element->phone, phone);
	strcpy(element->address, address);
	element -> next = NULL;
	element -> prev = NULL;
	return element;
}

void deleteBook(contact **head)
{
	contact *p;
	while(*head != NULL){
		p = (*head) -> next;
		free(*head);
		*head = p;
	}
}

void insertElement(contact **head, contact *element)
{
	if(*head == NULL)
	{
		*head = element;
		(*head )-> next = NULL;
		(*head )-> prev = NULL;
		return; 
	}
	element-> next = *head;
	element -> prev = NULL;
	(*head) -> prev = element;
	*head = element;	
}

void deleteElement(contact ** head, contact *element)
{
	if( element == NULL) 
		return;
	contact * p = (element) -> next;				// if this element is first in list
 													// remeber the next of him
	if(element == *head){	
		*head = (*head) -> next; 
		if( p != NULL)		
			p -> prev = *head;						// if has successor, change his predessor to null;
	} else	{ 										// if element has element before him
		element -> prev -> next = element -> next;
		if(element-> next != NULL)
			element ->next -> prev = element -> prev;
	}
	
	free(element);					
}

contact *findElement(contact *head, char * text)
{  // find fists element that containst that 
	while(head != NULL ){
		if(strcmp(head->name, text) == 0  ||
			strcmp(head->surname, text) == 0 ||
			strcmp(head->birthDate, text) == 0 ||
			strcmp(head->phone, text) == 0 ||
			strcmp(head->address, text) == 0 )
			return head;
		head = head -> next;
	}
	return NULL;
}

void sortBook(contact **head)
{
	if(*head == NULL || (*head) -> next == NULL ) 	// checking are lazy
		return;
	contact *p = *head;
	contact *cur;
	contact *smaller = NULL;
	contact *bigger = NULL;
	contact *x = NULL;
	cur = (*head) -> next;
	p -> next = NULL;
	p -> prev = NULL;
	while( cur != NULL)
	{
		x = cur -> next;
		int n = strcmp(cur-> surname, p->surname );
		if ( n > 0 )
			insertElement(&bigger,cur);
		else if( n < 0)
			insertElement(&smaller, cur);
		else
			insertElement(&p,cur);
		cur = x;
	}
	sortBook(&smaller);
	sortBook(&bigger);
	cur = smaller;									// finding the last element of smaller list
	while(cur != NULL && cur -> next != NULL)
	{
		cur  = cur-> next;
	}												
	if(cur != NULL)
	{
		cur -> next = p;
		p -> prev = cur;
		*head = smaller; 
	} else 
	{
		*head = cur = p;
	}
		
	while(cur != NULL && cur -> next != NULL )
		cur = cur -> next;    
	cur -> next = bigger;
	if( bigger != NULL)
		bigger -> prev = cur;
}

// TESTS\
void print(contact *head)
{
	while(head != NULL)
	{
		printf("###### \n The name is: %s\n The Surname is: %s \n The birthDate is: %s \n The phone is: %s\n The address is: %s\n ######\n",
			head->surname,
			head->name,
			head->birthDate,
			head->phone,
			head->address);
		head = head -> next;
	}
}
void printReverse(contact * head)
{
	while(head != NULL &&  head -> next != NULL)
		head = head -> next;
	while(head != NULL)
	{
		printf("%s\n", head->surname);
		head = head -> prev;
	}


}

// int main()
// {
// 	contact * head =createBook();
// 	int  n = 5;
// 	contact * p;
// 	for( int i = 0 ; i < n ; i++)
// 	{
// 		p = (contact*) malloc(sizeof(contact));
// 		scanf("%s",p->surname); 
// 		insertElement(&head, p);
// 	}

// 	printf("\n" );
// 	// deleteBook(&head); // WORKS
// 	char * tab;
// 	tab = "E";
// 	p = findElement(head,tab); // WORKS
// 	// sortBook(&head);
// 	deleteElement(&head, p);
// 	print(head);
// 	printf("\n" );
// 	// printReverse(head);


// }