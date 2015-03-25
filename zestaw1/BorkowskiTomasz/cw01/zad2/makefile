all: contactBook_dynamic contactBook_static contactBook_load run_test_dynamic run_test_static run_test_load 
CC=gcc
CFLAGS= -Wall -pedantic -std=c99 
LIBS = lcontact
#LIBPATH=.

run_test_static:
	echo "Static test:"
	./contactBook_static;
run_test_dynamic: contactBook_dynamic
	echo "Dynamic test:"
	./contactBook_dynamic
run_test_load: contactBook_load
	echo "Load Dynamic test: "
	./contactBook_load


contactBook_dynamic: contactBook.c
	$(CC) -o $@ contactBook.c -I. -L../zad1 $(CFLAGS) -Wl,-rpath ../zad1/ -lcontact

contactBook_static: contactBook.c
	$(CC) $(CFLAGS) -static -o $@ contactBook.c -I../zad1  -L../zad1 -$(LIBS) 

contactBook_load: contactBook_load.c
	$(CC) $^ -ldl -o $@ -Wall -std=c99 

clean:
	rm contactBook_dynamic contactBook_static contactBook_load
