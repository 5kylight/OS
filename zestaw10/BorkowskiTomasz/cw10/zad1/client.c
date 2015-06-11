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
void *pinger_thread(void *);

enum con_type type; 
int client_socket;
char *user_id;

int working = 1;

int main(int argc, char *argv[])
{	
	printf("Hello in chat! Type: \'exit\' or \'Ctrl + C\' to exit \n");

	/* Arguments */
	if (argc != 4 && argc != 5)
		usage(argv[0]);

 	/* Prepare to exit */
	signal(SIGINT, before_exit);


	user_id = argv[1];
	get_type(argv[2], argc, &type);
	pthread_t reader;
	pthread_t pinger;

	struct sockaddr *recv_server;
	
	if(type == LOCAL) {
		/* Get socket path*/
		char *socket_path = argv[3];

		/* Server socket */
		struct sockaddr_un sa_server_un;
		strcpy(sa_server_un.sun_path, socket_path);
		sa_server_un.sun_family = AF_UNIX;

		/* Client socket */
		unlink(user_id);
		struct sockaddr_un sa_client_un;
		strcpy(sa_client_un.sun_path, user_id);
		sa_client_un.sun_family = AF_UNIX;
		client_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
		if (bind(client_socket, (struct sockaddr *) &sa_client_un, sizeof(sa_client_un)) < 0)
			error("client unix bind");
		
		/* Register client */
		recv_server = (struct sockaddr *) &sa_server_un;

		/* Start sending */
		pthread_create(&reader, NULL, &reader_thread, (struct sockaddr *) &sa_server_un);

		/* Start pinging */
		pthread_create(&pinger, NULL, &pinger_thread, (struct sockaddr *) &sa_server_un);
		/* REMOTE */
	} else {	

		/* Arguments */
		int port;
		char *address = argv[3];
		port = atoi(argv[4]);
		
		/* Server socket*/
		struct sockaddr_in sa_server_in, sa_client_in;
	    sa_server_in.sin_family = AF_INET;
		sa_server_in.sin_port = htons(port);
		struct hostent *server;
	    server = gethostbyname(address);
	    bcopy(server -> h_addr, &sa_server_in.sin_addr.s_addr, server -> h_length);

	    /* Client socket */
	    sa_client_in.sin_family = AF_INET;
	    srand(time(NULL));
	    sa_client_in.sin_port = htons(port +  rand()%100);
	    sa_client_in.sin_addr.s_addr = INADDR_ANY;
		client_socket = socket(AF_INET, SOCK_DGRAM, 0);

     	if (bind(client_socket, 
     			(struct sockaddr *) &sa_client_in,
              	sizeof(sa_client_in)) < 0)
			error("client inet bind");

		recv_server = (struct sockaddr *) &sa_server_in;

		/* Start sending */
		pthread_create(&reader, NULL, &reader_thread, (struct sockaddr *) &sa_server_in);

		/* Start pinging */
		pthread_create(&pinger, NULL, &pinger_thread, (struct sockaddr *) &sa_server_in);
	}

		/* Start receiving */
		struct pollfd ufd;
		struct message message;
		socklen_t sa_len;

		int rv;
		while(working) {
			ufd.fd = client_socket;
			ufd.events = POLLIN;

			if ((rv = poll(&ufd, 1, 3000))== -1) {
			    error("poll");
			} else {
				sa_len = sizeof(*recv_server);
				if (recvfrom(client_socket, 
										&message, 
										sizeof(message),
										O_NONBLOCK,
										recv_server,
										&sa_len) < 0)
					error("recvform");
				printf("\t------------\n");
				printf("%s:\n\t%s\n",message.client_name, message.msg);
				printf("\t------------\n");
			}
		}
	pthread_join(reader, NULL);
}

void *reader_thread(void *serv)
{
	struct sockaddr *sa_server = (struct sockaddr *) serv;
	struct message message;

	/* Register client */
	message.msg_type = NEW_CLIENT;
	strcpy(message.client_name, user_id);
	if (sendto(client_socket, 
					&message, 
					sizeof(message), 
					0, 
					sa_server, 
					sizeof(*sa_server)) < 0)
			error("sendmsg");

	/* Start reading */
	message.msg_type = MSG;
	
	/* While it is not "END" */
	do {	
		char c;
		int i;
		/* Reading from stdin*/
		for(i = 0; i < MSG_SIZE && (c = fgetc(stdin)) != '\n'; i++)
			message.msg[i] = c;
		message.msg[i] = (char) 0;

		if (sendto(client_socket, 
					&message, 
					sizeof(message), 
					0, 
					sa_server, 
					sizeof(*sa_server)) < 0)
			error("sendmsg");

	} while(strcmp(message.msg, "exit"));	

	printf("== Finished ==\n");
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

/* Sends message with msg_type==PING to server in every MAX_TIME - 1*/
void *pinger_thread(void *serv)
{	
	struct sockaddr *sa_server = (struct sockaddr *) serv;
	struct message message;
	message.msg_type = PING;
	strcpy(message.client_name, user_id);

	while (working){
		sleep(MAX_TIME - 1);
		if (sendto(client_socket, 
					&message, 
					sizeof(message), 
					0, 
					sa_server, 
					sizeof(*sa_server)) < 0)
			error("sendmsg");
	}
	return NULL;
}


void before_exit()
{	
	working = 0;
	shutdown(client_socket, 2);
	if (type == LOCAL)
		remove(user_id);
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