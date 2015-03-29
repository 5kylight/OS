all: lock_byte
CC=gcc 
CFLAGS= -g -std=gnu99 -Wall -pedantic 

lock_byte: lock_byte.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm lock_byte

