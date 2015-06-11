#include <fcntl.h> 
#include <netinet/in.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define h_addr h_addr_list[0] /* for backward compatibility */
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#define MSG_SIZE 256
#define DEFAULT_CLIENTS 2
#define CAPACITY_FACTOR 0.75
#define MAX_CLIENTS 10
#define MAX_TIME 30

enum msg_type{NEW_CLIENT, REMOVE_CLIENT, MSG, EXIT};
enum client_type{UNIX, INET};

struct message
{
	char client_name[20];
	enum msg_type msg_type;
	char msg[MSG_SIZE];
};

struct client
{	
	char client_name[20];
	struct sockaddr client_sa;
	enum client_type client_type;
	time_t last_access;
};