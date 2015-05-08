#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "config.h"

struct client{
    mqd_t queue;
    char name[MAX_CLIENT_NAME];
    int is_active;
};

void signalHandler(int);
int addClient(char*);
void send_back(char*, int);
void before_exit();

mqd_t server;
struct mq_attr attr;
int working;
int client_number;
struct client clients[MAX_CLIENTS];

int 
main(int argc, char* argv[])
{
    working = 1;
    client_number = 0;

    /* Prepare to recieve signal */
    if(signal(SIGINT, signalHandler) == SIG_ERR)
        perror("signal");
    
    printf("\n#\tTo quit: \" Ctrl + C \" \n\n");
    atexit(before_exit);

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 3000;
    attr.mq_flags = O_NONBLOCK;

    int flags = O_RDONLY | O_CREAT | O_NONBLOCK;
    int perm = S_IRUSR | S_IWUSR;

    if((server = mq_open("/serverqueue", flags, perm, &attr)) == -1)
        perror("mq_open server queue");

    char message[MSG_SIZE + 1];
    unsigned int piority;

    while(working)
    {
        if(mq_receive(server, message, MSG_SIZE, &piority) == -1)
        {
            if(errno != EAGAIN)
            {
                perror("mq_receive()");
                working = 0;
            }
        }
        else
        {
            char text[100];
            int space = 0;
            char *pch;
            pch = (char*)memchr(message, ' ', strlen(message));
            space = pch - message;
            strncpy(text, message, space);
            text[space] = '\0';

            if(strcmp(text, "!!new") == 0)
            {
                char text2[100];
                strncpy(text2, message + space + 1, strlen(message) - space + 1);
                if(addClient(text2) < 0)
                {
                    printf("Bad client name\n");
                }
                else
                {   
                    char *info = "\n#\n#\tSERVER: new client %s\n#\n";
                    char registred[strlen(info) + strlen(text2) + 1];
                    sprintf(registred, info, text2);
                    printf(info, text2);
                    send_back(registred, -1);
                }
            }
            else
            {
                int i = 0;
                int j;
                char text2[100];
                strncpy(text2, message, space);
                text2[space] = '\0';
                for(j=0; j < client_number; ++j)
                {
                    if(strcmp(text2, clients[j].name) == 0)
                        i = j;
                }
                send_back(message, i);
                
                /* Print message to console on server */
                printf("%s\n", message);
            }
        }
        usleep(8000);
    }
    return 0;
}

void 
signalHandler(int signal)
{   
    printf("\nExiting\n");
    working = 0;

    exit(EXIT_SUCCESS);
}

int 
addClient(char* name)
{
    strcpy(clients[client_number].name, name);
    clients[client_number].is_active = 1;

    char queue_name[110];
    sprintf(queue_name, "/%squeue", name);

    int perm = O_WRONLY | O_NONBLOCK;

    if((clients[client_number].queue = mq_open(queue_name, perm)) == -1)
        perror("mq_open client");

    client_number++;

    return 0;
}


void 
send_back(char* text, int i)
{
    int j;
    for(j=0; j < client_number; ++j)
    {
        if((j != i) && (clients[j].is_active))
        {
            if(mq_send(clients[j].queue, text, strlen(text) + 1, 0) < 0)
            {
                perror("mq_send()");
                working = 0;
            }
        }
    }
}

void 
before_exit()
{
    if(mq_unlink("/serverqueue") < 0)
        perror("mq_unlink");
}