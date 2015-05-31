CC = gcc
CFLAGS = -Wall -g -lrt -std=c99
LFLAGS = -lrt -pthread

OBJS = readers_and_writers.o 

all:$(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $@ -o $* $(LFLAGS)

.PHONY : clean
clean :
	rm -f readers_and_writers.o readers_and_writers
