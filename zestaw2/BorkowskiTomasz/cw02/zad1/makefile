all: generate sort_byte test
CC=gcc 
CFLAGS=-std=c99 -Wall -pedantic 

generate: generate.c
	$(CC) $(CFLAGS) $^ -o $@

sort_byte: sort_byte.c
	$(CC) $(CFLAGS) $^ -o $@

test:
	./generate a 4 2000
	cp a a1
	./sort_byte a 4
	./generate a 4 3000
	cp a a1
	./sort_byte a 4

	./generate a 512 2000
	cp a a1
	./sort_byte a 512
	./generate a 512 3000
	cp a a1
	./sort_byte a 512

	./generate a 4096 2000
	cp a a1
	./sort_byte a 4096
	./generate a 4096 3000
	cp a a1
	./sort_byte a 4096

	./generate a 8192 2000
	cp a a1
	./sort_byte a 8192
	./generate a 8192 3000
	cp a a1
	./sort_byte a 8192
