CC = gcc
CFLAGS = -Wall -g -std=c99 -pthread

all: search generate

search: search.c
	$(CC) $(CFLAGS) $^ -o $@
generate: generate.c
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm search generate 
