#include <stdio.h>
#include <stdlib.h>
#include "contact.h"
int main(int argc, char const *argv[])
{

	contact * head =createBook();
	int  n = 5;
	contact * p;
	
	for( int i = 0 ; i < n ; i++)
	{
		p = (contact*) malloc(sizeof(contact));
		scanf("%s",p->surname); 
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