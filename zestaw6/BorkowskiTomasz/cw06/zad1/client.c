#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "config.h"

struct mymesg{
	long mtype;
	char mtext[MAX_TEXT_SIZE];
};

pid_t pid;
int server;
int client;
char *queue;

int
main(int argc, char *argv[])
{
	
	if(argc != 2)
	{
		printf("Invalid arguments!\n Usage:\n\t%s\t<your_name>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if(strlen(argv[1]) + 1 > MAX_CLIENT_NAME)
	{
		printf("Your name is too long! \n Usage:\n\t%s\tyour_name\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pid = getpid();

	key_t client_key;
	if((client_key = ftok(".", pid)) == -1)
		perror("client ftok");

	if((client = msgget(client_key, IPC_CREAT | 0600)) < 0)
		perror("client msget");

	key_t server_key;
	if((server_key = ftok(".", 1)) == -1)
		perror("server ftok");

	if((server = msgget(server_key, 0200)) < 0)
		perror("msget server");

	printf("\n#\tHello in chat!\n#\tSuccesfully connected to server.");
	printf("\n#\tTo quit type \" end\"\n\n");

	struct mymesg message;
	message.mtype = pid;
	strcpy(message.mtext, argv[1]);

	if(msgsnd(server, &message, MAX_CLIENT_NAME, 0) < 0)
		perror("msgsnd()");

	pid_t writing_process = getpid();
	pid_t reading_process = fork();

	if(reading_process == 0){
		while(kill(writing_process, 0) == 0)
		{		
			sleep(1);

			if(msgrcv(client, &message, MAX_TEXT_SIZE, SERVER_CLIENT, IPC_NOWAIT) < 0)
			{
				if(errno == EAGAIN || errno == ENOMSG)	
					continue;				
				else					
					perror("reading process msgrcv");	
			}

			printf("%s", message.mtext);
		}

		if(msgctl(client, IPC_RMID, (struct msqid_ds *) NULL) < 0)
			perror("atexit_function  msgctl");
		exit(EXIT_SUCCESS);
	}


	message.mtype = CLIENT_SERVER;
	do 
	{
		char c;
		int i;
		for(i = 0; i < MAX_TEXT_SIZE && (c = fgetc(stdin)) != '\n'; i++)
			message.mtext[i] = c;
		
		message.mtext[i] = (char) 0;

		if(msgsnd(client, &message, MAX_TEXT_SIZE , 0) < 0)
			perror("msgsnd ");

	} while(strcmp(message.mtext, "end"));

	exit(EXIT_SUCCESS);

}
