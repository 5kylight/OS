CC = gcc
CFLAGS = -Wall -g -lrt 
LFLAGS = -lrt -pthread

OBJS = main.o producer.o consumer.o 

all:$(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $@ -o $* $(LFLAGS)

.PHONY : clean
clean :
	rm -f consumer producer producer.o consumer.o main main.o
