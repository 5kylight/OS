CC = gcc
CFLAGS = -Wall -g -std=c99 

all : consumer producer main

consumer: consumer.c 
	$(CC) $(CFLAGS) $^ -o $@

producer: producer.c 
	$(CC) $(CFLAGS) $^ -o $@

main: main.c
	$(CC)  $(CFLAGS) $^ -o $@

.PHONY : clean
clean :
	rm -f consumer producer consumer.o producer.o main.o main
