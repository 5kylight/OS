CC = gcc
CFLAGS = -Wall -g -lrt -std=c99 -pthread
LFLAGS = -lrt -pthread

OBJS = client.o server.o

all:$(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $@ -o $* $(LFLAGS)

.PHONY : clean
clean :
	rm -f client.o client server server.o
