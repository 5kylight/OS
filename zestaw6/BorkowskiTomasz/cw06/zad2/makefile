CC = gcc
CFLAGS = -Wall -g -lrt
LFLAGS = -lrt
OBJS = server.o client.o

all : $(OBJS) run

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -o $* $@ $(LFLAGS)
run:
	./server
.PHONY : clean
clean :
	rm -f $(OBJS) server client
.PHONY : clean-all
clean-all :
	@rm -f $(OBJS) $(OBJS:.o=)
