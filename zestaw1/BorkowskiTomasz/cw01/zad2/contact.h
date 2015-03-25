#ifndef CONTACT_H
#define CONTACT_H

typedef struct contact{
	struct contact *next;
	struct contact *prev;
	char name[15];
	char surname[20];
	char birthDate[8];
	char phone[10];
	char address[20];
} contact;

contact *createBook();
contact *createContact(char *name, char *surname, char *birthDate, char *phone, char *address);
void deleteBook(contact **head);
void insertElement(contact **head, contact *element);
void deleteElement(contact **head, contact *element);
contact  *findElement(contact *head, char *text);
void sortBook(contact **head);
void print(contact *head);
void printReverse(contact * head);

#endif