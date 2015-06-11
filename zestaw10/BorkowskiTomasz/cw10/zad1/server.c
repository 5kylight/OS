#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "chat.h"

void error(char *);
void usage(char *);
int is_registred(struct client);
void ensure_clients_capacity();
void before_exit();
void register_client(struct client);
void send_from(struct client, struct message);
void remove_client(struct client client);
void remove_client_at(int);
void update_last_access(struct client);

int inet_socket, unix_socket;
struct client* clients[MAX_CLIENTS];

int clients_number = 0;
int clients_size;
char *socket_path;
int main(int argc, char *argv[])
{	
	/* Arguments */
	if (argc != 3)
		usage(argv[0]);
	int port_number = atoi(argv[1]);
	socket_path = argv[2];

	/* Internet socket */
	struct sockaddr_in sa_server_in;
	sa_server_in.sin_family = AF_INET;
	sa_server_in.sin_addr.s_addr = INADDR_ANY;
	sa_server_in.sin_port = htons(port_number);
	inet_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (bind(inet_socket, (struct sockaddr *) &sa_server_in, sizeof(sa_server_in)) < 0)
		error("inet bind");
	
	/* Unix socket */
	struct sockaddr_un sa_server_un;
	strcpy(sa_server_un.sun_path, socket_path);
	sa_server_un.sun_family = AF_UNIX;
	unlink(socket_path);
	unix_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (bind(unix_socket, (struct sockaddr *) &sa_server_un, sizeof(sa_server_un)) < 0)
		error("unix bind");

	/* FD sets */
	fd_set set;

	int fd_hwm = inet_socket > unix_socket ? inet_socket : unix_socket;

	/* Prepare to exit */
	signal(SIGINT, before_exit);

	clients_size = MAX_CLIENTS;
	socklen_t sa_len;

	struct message message;
	struct client client;
	
	while (1) {	
		FD_ZERO(&set);
		FD_SET(inet_socket, &set);
		FD_SET(unix_socket, &set);

		/* Waiting */
		if (select(fd_hwm + 1, &set, NULL, NULL, NULL) < 0)
			error("select");

		/* Receiving from unix socket */
		if (FD_ISSET(unix_socket, &set)) {
			sa_len = sizeof(client.client_sa);
			if (recvfrom(unix_socket, 
							&message, 
							sizeof(message),
							O_NONBLOCK,
							&client.client_sa,
							&sa_len) < 0)
				error("recvform");
			client.client_type = UNIX;
		} 

		/* Receiving from intenet*/
		else if (FD_ISSET(inet_socket, &set)) {
			if (recvfrom(inet_socket, 
							&message,
							sizeof(message),
							O_NONBLOCK,
							&client.client_sa,
							&sa_len) < 0)
				error("recvform");
		
			client.client_type = INET;
		}
		/* Get client name*/
		strcpy(client.client_name, message.client_name);
		
		/* Deal with message*/
		if (message.msg_type == NEW_CLIENT)
		{
			register_client(client);
		}
		else if (message.msg_type == MSG) {
			printf("\t\t------------\n");
			printf("%s:\n\t%s\n", message.client_name, message.msg);
			printf("\t\t------------\n");
			update_last_access(client);
			send_from(client, message);
		} else if (message.msg_type == PING) {
			printf("Client %s:\tstill active\n", message.client_name);
			update_last_access(client);
		}
	}
}

/* Update last access time for client name */
void update_last_access(struct client client)
{
	for (int i = 0; i < clients_number; i++)
		if (!strcmp(clients[i] -> client_name, client.client_name)) {
			time(&(clients[i] -> last_access));
			break;
		}
}

void remove_client(struct client client)
{
	for (int i = 0; i < clients_number; i++)
		if (!strcmp(clients[i] -> client_name, client.client_name)) {
			remove_client_at(i);
			break;
		}
}

void remove_client_at(int index)
{		
	printf("%s unregistred!\n", clients[index] -> client_name);
	free(clients[index]);
	clients[index] = clients[--clients_number];
}

void send_from(struct client from, struct message message)
{
	time_t curr_time;
	time(&curr_time);

	for (int i = 0; i < clients_number; i++) {
		if (difftime(curr_time, clients[i] -> last_access) > MAX_TIME){
			remove_client_at(i);
			continue;
		}
		if (strcmp(clients[i] -> client_name, from.client_name)) {
			if (clients[i] -> client_type == UNIX) {
				if (sendto(unix_socket,  
						&message, 
						sizeof(message), 
						0, 
						&clients[i] -> client_sa, 
						sizeof(clients[i] -> client_sa)) < 0)
					error("sendmsg send_from unix");
			} else {
				if (sendto(inet_socket, 
						&message, 
						sizeof(message), 
						0, 
						&clients[i] -> client_sa, 
						sizeof(clients[i] -> client_sa)) < 0)
					error("sendmsg send_from inet");
			}
		}
	}			
}

void register_client(struct client client)
{	
	if(!is_registred(client) && clients_number + 1 < clients_size) {
		struct client *tmp_client = malloc(sizeof(struct client));
		tmp_client -> client_sa = client.client_sa;
		tmp_client -> client_type = client.client_type;
		strcpy(tmp_client -> client_name, client.client_name);
		time(&(tmp_client -> last_access));
		clients[clients_number++] = tmp_client;
		printf("%s\t-registred!\n", client.client_name);
	} else {
		printf("%s client exists \n", client.client_name);
	}
}

int is_registred(struct client client)
{
	for (int i = 0; i < clients_number; i++)
		if (clients[i] -> client_name == client.client_name)
			return 1;
	return 0;
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

void before_exit()
{
	shutdown(inet_socket, 2);
	shutdown(unix_socket, 2);
	remove(socket_path);
	exit(0);
}