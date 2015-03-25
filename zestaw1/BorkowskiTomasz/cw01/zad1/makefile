all: libcontact.so libcontact.a

CC=gcc
CFLAGS = -fPIC -Wall -pedantic -std=c99

libcontact.so: contact.o
	$(CC) -shared -o libcontact.so $< $(CFLAGS) 

libcontact.a: contact.o
	ar crs libcontact.a contact.o 

contact.o: contact.c 
	gcc  -c contact.c $(CFLAGS) -I.

clean:
	rm contact.o libcontact.*
