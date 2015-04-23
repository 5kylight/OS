all: pipes run
CC = gcc
CFLAGS =-g -std=c99 -Wall -pedantic

pipes: pipes.c
	$(CC) $(CFLAGS) -o $@ $^
run:	
	./pipes		
clean:
	rm pipes 
