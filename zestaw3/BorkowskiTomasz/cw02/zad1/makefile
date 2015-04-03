all: forking_tests_* run_tests
CC = gcc
CFLAGS =-g -std=c99 -Wall -pedantic


forking_tests_clone: forking_tests.c
	$(CC) $(CFLAGS) $^ -o $@ -DCLONE
	
forking_tests_vclone: forking_tests.c
	$(CC) $(CFLAGS) $^ -o $@ -DVCLONE

forking_tests_fork: forking_tests.c
	$(CC) $(CFLAGS) $^ -o $@ -DFORK

forking_tests_vfork: forking_tests.c
	$(CC) $(CFLAGS) $^ -o $@ -DVFORK

run_tests:
	./forking_tests_* 

clean:
	rm forking_tests_*
