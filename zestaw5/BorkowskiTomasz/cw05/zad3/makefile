all: popen run
CC = gcc
CFLAGS =-g -std=c99 -Wall -pedantic

popen: popen.c
	$(CC) $(CFLAGS) -o $@ $^
run:	
	./popen		
clean:
	rm popen folders.txt
