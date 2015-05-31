CC = gcc
CFLAGS = -Wall -g -lrt -std=c99
LFLAGS = -lrt -pthread

OBJS = five_philosophers.o 

all:$(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $@ -o $* $(LFLAGS)

.PHONY : clean
clean :
	rm -f five_philosophers.o five_philosophers
