#define _SVID_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "config.h"

#define FROM_SERVER -1


struct mymesg{
	long mtype;
	char mtext[MAX_TEXT_SIZE];
};

struct client{
	int queue;
	pid_t pid;
	char name[MAX_CLIENT_NAME];
	int is_active;
};


int server;
int working;
int client_number;
struct client clients[MAX_CLIENTS];

void signal_handler(int);
void add_client(struct mymesg*);
void send_back(const char*, int, time_t);
void delete_client(int);
void before_exit();

int
main(int argc, char *argv[])
{	
	working = 1;
	client_number = 0;
	atexit(before_exit);
    
    /*  to recieve signal */
	if(signal(SIGINT, signal_handler) == SIG_ERR)
		perror("signal");
	printf("\n#\tTo quit: \" Ctrl + C \"\n\n");

	key_t server_key;
	struct mymesg message;

	if((server_key = ftok(".", 1)) == -1)
		perror("ftok");

	if((server = msgget(server_key, IPC_CREAT | 0600)) < 0)
		perror("open queue");

	while(working)
	{
        usleep(8000);

		if(msgrcv(server, &message, MAX_CLIENT_NAME, 0, IPC_NOWAIT) < 0)
		{
			if(errno == E2BIG)
				printf("Name is too long\n");
			else if(errno != EAGAIN && errno != ENOMSG)
				perror("No message!");
		} 
		else 
		{
			add_client(&message);
			char *info = "\n#\n#\tSERVER: %s is active\n#\n";
			char registred[strlen(info) + strlen(message.mtext) + 1];
			sprintf(registred, info, message.mtext);

			send_back(registred, (int) FROM_SERVER, (time_t) 0);
		}

		time_t tm;

		for(int i = 0; i < client_number; i++)
		{
			if(clients[i].is_active)
			{
				if((tm = time(NULL)) == -1)
					perror("Time error");
				if(msgrcv(clients[i].queue, &message, MAX_TEXT_SIZE, CLIENT_SERVER, IPC_NOWAIT) < 0)
				{
					if(errno == EAGAIN || errno == ENOMSG)
						continue;
					if(errno == EINVAL)
					{
						delete_client(i);
						char *end_info = "\n#\n#\tSERVER: %s is not active\n#\n";
						char removed[strlen(end_info) + strlen(clients[i].name + 1)];
						sprintf(removed, end_info, clients[i].name);
						send_back(removed, -1, (time_t) 0);
						errno = EAGAIN;
					}
				}
				else
				{
					if(strcmp(message.mtext, "end") != 0)
						send_back(message.mtext, i, tm);
				}
			}
		}
	}
	exit(EXIT_SUCCESS);
}


void
add_client(struct mymesg *message)
{
	key_t tmp;
	clients[client_number].pid = message -> mtype;
	clients[client_number].is_active = 1;

	strcpy(clients[client_number].name, message -> mtext);
	if((tmp = ftok(".", message -> mtype)) == -1)
		perror("add_client ftok");

	if((clients[client_number].queue = msgget(tmp, 0600)) < 0)
		perror("add_client msget");

	client_number++;
}


void
delete_client(int client)
{
	clients[client].is_active = 0;
}

void
send_back(const char *mtext, int from, time_t tm)
{
	struct mymesg message;
	message.mtype = SERVER_CLIENT;

	if(from == FROM_SERVER)
		strcpy(message.mtext, mtext);
	else
		sprintf(message.mtext, "\n\t%s \t\t%s-> %s\n\n", 
			clients[from].name, 
			ctime(&tm), 
			mtext);

	printf("\t%s\n", message.mtext);

	for(int i = 0; i < client_number; i++)
		if(i != from 
			&& clients[i].is_active 
			&& msgsnd(clients[i].queue, &message, MAX_TEXT_SIZE, 0) < 0)
				perror("Can't send message");	
}	

void
signal_handler(int signo)
{
	exit(EXIT_SUCCESS);
}

void
before_exit()
{
	if(msgctl(server, IPC_RMID, (struct msqid_ds *) NULL) < 0)
		perror("before_exit mgsctl");
}