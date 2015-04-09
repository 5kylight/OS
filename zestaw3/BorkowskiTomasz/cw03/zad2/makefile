all: file_counter run_tests
CC = gcc
CFLAGS = -std=gnu99 -Wall -pedantic



file_counter: file_counter.c
	$(CC) $(CFLAGS) $^ -o $@

run_tests:
	./file_counter ~
run_tests_w:
	./file_counter ~ -w
clean:
	rm file_counter
