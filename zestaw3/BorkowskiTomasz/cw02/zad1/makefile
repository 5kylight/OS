all: forking_tests_clone forking_tests_vclone forking_tests_vfork forking_tests_fork run_tests
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
	./forking_tests_clone 40000
	./forking_tests_vclone 40000
	./forking_tests_vfork 40000
	./forking_tests_fork 40000
	./forking_tests_clone 50000 
	./forking_tests_vclone 50000 
	./forking_tests_vfork 50000 
	./forking_tests_fork 50000 
	./forking_tests_clone 60000
	./forking_tests_vclone 60000
	./forking_tests_vfork 60000
	./forking_tests_fork 60000
	./forking_tests_clone 70000
	./forking_tests_vclone 70000
	./forking_tests_vfork 70000
	./forking_tests_fork 70000

	

		
		
clean:
	rm forking_tests_*
	rm ./res/*
	rm ./plot/*.svg
