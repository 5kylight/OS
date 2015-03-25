all: generate sort_byte test
CC=gcc 
CFLAGS=-std=c99 -Wall -pedantic 

generate: generate.c
	$(CC) $(CFLAGS) $^ -o $@

sort_byte: sort_byte.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./generate a 1 1000
	./sort_byte a 1
