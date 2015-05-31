CC = gcc
CFLAGS = -Wall -g -std=c99 -pthread

all: search 

search: search.c
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm search
