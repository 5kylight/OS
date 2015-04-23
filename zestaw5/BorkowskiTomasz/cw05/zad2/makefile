all: rdfifo wrfifo 
CC = gcc
CFLAGS =-g -std=c99 -Wall -pedantic

rdfifo: rdfifo.c
	$(CC) $(CFLAGS) -o $@ $^
wrfifo: wrfifo.c
	$(CC) $(CFLAGS) -o $@ $^	
run:	
	./rdfifo fifo 
clean:
	rm wrfifo rdfifo 
