CC = gcc
CFLAGS = -Wall -g -std=c99 -pthread
LFLAGS = -lrt -pthread

all: search.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $@ -o $* $(LFLAGS)

clean:
	rm search search.o
