#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "chat.h"

void error(char *);
void usage(char *);
void before_exit();
void send_from(int, struct message);

int inet_socket, unix_socket;
struct client* clients[MAX_CLIENTS];

int clients_number = 0;
int clients_size;
char *socket_path;
fd_set receivers;
int receivers_hwm = 0;

int main(int argc, char *argv[])
{	
	/* Arguments */
	if (argc != 3)
		usage(argv[0]);
	int port_number = atoi(argv[1]);
	socket_path = argv[2];

	/* Prepare to exit */
	signal(SIGINT, before_exit);
	clients_size = MAX_CLIENTS;

	/* Internet socket */
	struct sockaddr_in sa_server_in;
	sa_server_in.sin_family = AF_INET;
	sa_server_in.sin_addr.s_addr = INADDR_ANY;
	sa_server_in.sin_port = htons(port_number);
	inet_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(inet_socket, (struct sockaddr *) &sa_server_in, sizeof(sa_server_in)) < 0)
		error("inet bind");
	if(listen(inet_socket, SOMAXCONN) < 0)
		error("listen inet");
	
	/* Unix socket */
	struct sockaddr_un sa_server_un;
	strcpy(sa_server_un.sun_path, socket_path);
	sa_server_un.sun_family = AF_UNIX;
	unlink(socket_path);
	unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (bind(unix_socket, (struct sockaddr *) &sa_server_un, sizeof(sa_server_un)) < 0)
		error("unix bind");
	if(listen(unix_socket, SOMAXCONN) < 0)
		error("listen");

	/* FD sets */
	fd_set set, read_set;
	FD_ZERO(&set);
	FD_ZERO(&receivers);

	FD_SET(inet_socket, &set);
	FD_SET(unix_socket, &set);
	int fd_hwm = inet_socket > unix_socket ? inet_socket : unix_socket;

	struct message message;
	int new_client;
	
	while (1)
	{	
		read_set = set;

		/* Waiting */
		if (select(fd_hwm + 1, &read_set, NULL, NULL, NULL) < 0)
			error("select");

		for (int fd = 0; fd <= fd_hwm; fd++) {
			if(FD_ISSET(fd, &read_set)) {

				/* If this is new connection to accept*/
				if (fd == unix_socket || fd == inet_socket) {
					new_client = accept(fd, NULL, 0);
					FD_SET(new_client, &set);
					FD_SET(new_client, &receivers);
					fd_hwm = new_client > fd_hwm ? new_client : fd_hwm;
					receivers_hwm = new_client > receivers_hwm ? new_client : receivers_hwm;
				}

				/* If this client is ready to read from him */
				else {
					recv(fd, &message, sizeof(message), 0);
					if (message.msg_type == NEW_CLIENT)
					{
						printf("Client %s:\t registred\n", message.client_name);
					}
					else if(message.msg_type == MSG){
						printf("\t\t------------\n");
						printf("%s:\n\t%s\n", message.client_name, message.msg);
						printf("\t\t------------\n");
						send_from(fd, message);
					} else if(message.msg_type == REMOVE_CLIENT){
						FD_CLR(fd, &receivers);
						FD_CLR(fd, &set);
					}
				}
			}
		}
	}
}

void send_from(int from, struct message message)
{
	for(int i = 0; i <= receivers_hwm; i++){
		if(FD_ISSET(i, &receivers) && i != from) {
			send(i, &message, sizeof(message), 0);
		}
	}			
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void usage(char *prog_name)
{
	printf("Usage:\n");
	printf("\t%s <PORT> <SOCKET PATH>\n", prog_name);
	exit(1);
}

void kill_all_clients(){
	struct message message;
	message.msg_type = EXIT;
	send_from(-1, message);
}

void before_exit()
{	
	kill_all_clients();
	shutdown(inet_socket, 2);
	shutdown(unix_socket, 2);
	remove(socket_path);
	exit(0);
}