#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"


void addElement(contact ** head, contact * element){
	 contact *p = malloc(sizeof(contact));
	printf("Say your name");

	scanf("%s",p->name);
	p -> next = *head;
	p -> prev = NULL;
		(*head) -> prev = p;
	*head = p;
}

void cout(contact * head){
	while(head != NULL){
		printf("%s\n",head -> name );
		head = head -> next;
	}
}

int main(int argc, char *argv[])
{
	contact * head;

	printf("how many contacts do you want\n");
	int n;
	scanf("%d",&n);
	head = malloc(sizeof(head));
	for(int i = 0 ; i < n ; i++)
		addElement(&head);
	printf("\n" );
	cout(head);

}