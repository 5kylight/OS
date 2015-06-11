#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include "chat.h"

enum con_type{REMOTE, LOCAL};

void error(char *);
void usage(char *);
void get_type(char *, int, enum con_type *);
void before_exit();
void *reader_thread(void *);

enum con_type type; 
int client_socket;
char *user_id;

int working = 1;
int exited_normaly = 1;

int main(int argc, char *argv[])
{
	printf("Hello in chat! Type: \'exit\' or \'Ctrl + C\' to exit \n");

	/* Arguments */
	if (argc != 4 && argc != 5)
		usage(argv[0]);

 	/* Prepare to exit */
	signal(SIGINT, before_exit);
	atexit(before_exit);

	user_id = argv[1];
	get_type(argv[2], argc, &type);
	pthread_t reader;

	if(type == LOCAL) {
		char *socket_path = argv[3];

		/* Server socket */
		struct sockaddr_un sa_server_un;
		strcpy(sa_server_un.sun_path, socket_path);
		sa_server_un.sun_family = AF_UNIX;

		client_socket = socket(AF_UNIX, SOCK_STREAM, 0);

		if (connect(client_socket, 
					(struct sockaddr *) &sa_server_un, 
					sizeof(sa_server_un)) < 0)
			error("conenct");
	} else {	
		
		/* Server socket*/
		int port;
		char *address = argv[3];
		port = atoi(argv[4]);

		struct sockaddr_in sa_server_in;
	    sa_server_in.sin_family = AF_INET;
		sa_server_in.sin_port = htons(port);
		struct hostent *server;
	    server = gethostbyname(address);
	    bcopy(server -> h_addr, &sa_server_in.sin_addr.s_addr, server -> h_length);

		client_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (connect(client_socket, 
					(struct sockaddr *) &sa_server_in, 
					sizeof(sa_server_in)) < 0)
			error("connect");
	}

		/* Start sending */
		pthread_create(&reader, NULL, &reader_thread, NULL);

		/* Start receiving */
		struct pollfd ufd;
		struct message message;

		int rv;
		while(working)
		{
			ufd.fd = client_socket;
			ufd.events = POLLIN;

			if ((rv = poll(&ufd, 1, 3000))== -1) {
			    perror("poll"); 
			} else {
				if (recv(client_socket, &message, sizeof(message), 0) < 0)
					error("recvform");

				if (message.msg_type == MSG) {
				printf("\t------------\n");
				printf("%s:\n\t%s\n", message.client_name, message.msg);
				printf("\t------------\n");
				
				/*If server finished working*/
				} else if (message.msg_type == EXIT) {
					printf("== Server finished job == \n");
					exited_normaly = 0;
					exit(0);
				}
			}
		}
	pthread_join(reader, NULL);
}

void *reader_thread(void *u)
{
	struct message message;

	/* Register client */
	message.msg_type = NEW_CLIENT;
	strcpy(message.client_name, user_id);
	if (send(client_socket, 
			&message, 
			sizeof(message), 
			0) < 0)
		error("sendmsg");

	/* Start reading */
	message.msg_type = MSG;
	
	/* While it is not "exit" */
	do 
	{	
		char c;
		int i;
		for(i = 0; i < MSG_SIZE && (c = fgetc(stdin)) != '\n'; i++)
			message.msg[i] = c;
		
		message.msg[i] = (char) 0;

		if (send(client_socket, 
				&message, 
				sizeof(message), 
				0) < 0)
			error("sendmsg");

	} while(strcmp(message.msg, "exit"));	

	exit(0);
}

void get_type(char *arg, int argc, enum con_type *type)
{
	if ((!strcmp(arg, "R") || !strcmp(arg, "REMOTE")) && argc == 5 )
		*type = REMOTE;
	else if ((!strcmp(arg, "L") || !strcmp(arg, "LOCAL")) && argc == 4 )
		*type = LOCAL;
	else
		usage("client");
}

void before_exit()
{	
	if (exited_normaly) {
		struct message message;
		message.msg_type = REMOVE_CLIENT;
		if (send(client_socket, 
					&message, 
					sizeof(message), 
					0) < 0)
				error("sendmsg");		
	}
	close(client_socket);
	exit(0);
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void usage(char *prog_name)
{
	printf("Usage:\nFor local:\n");
	printf("\t%s <User ID> <REMOTE | R> <IP> <PORT>\n", prog_name);
	printf("For internet:\n");
	printf("\t%s <User ID> <LOCAL | L> <SOCKET PATH>\n", prog_name);
	exit(1);
}