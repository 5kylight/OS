all: server client run
CC = gcc
CFLAGS =-g -std=c99 -Wall -pedantic

server: server.c
	$(CC) $(CFLAGS) -o $@ $^
client: client.c
	$(CC) $(CFLAGS) -o $@ $^
run:	
	./server		
clean:
	rm server client 
