#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>
#include <unistd.h>
#include "config.h"

#define MAX_TEXT_SIZE 4096
#define MAX_CLIENT_NAME 100
#define MSG_SIZE 4000

mqd_t server;
mqd_t client;
int working;

pid_t pid;
pid_t child;
char* name;

struct mq_attr attr;
unsigned int piority;

void signalHandler(int signal);
void addClient();
void before_exit();


int 
main(int argc, char* argv[])
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
    atexit(before_exit);


    name = argv[1];
    working = 1;

    /* Prepare to recieve signal */
    if(signal(SIGINT, signalHandler) == SIG_ERR)
  		perror("signal");

    char message[MSG_SIZE + 1];
    pid = getpid();

    if((server = mq_open("/serverqueue", O_WRONLY)) == -1)
        perror("mq_open");

    addClient();


	printf("\n#\tHello in chat!\n#\tSuccesfully connected to server.");
	printf("\n#\tTo quit: \" Ctrl + C \" \n\n");

    child = fork();
    if(child > 0)
    {
        while(working)
        {
            usleep(500000);
            if(mq_receive(client, message, MSG_SIZE, NULL) < 0)
            {
                if(errno != EAGAIN)
                {
                	perror("mq_receive");
                    working = 0;
                }
            }
            else
            {
                printf("\n\t%s\n", message);
            }
        }
        exit(0);
    }
    else if(child == 0)
    {
        while(working)
        {
            char ch = '\n';
            int i;

            /* Read message */
            for(i = 0; i < MAX_TEXT_SIZE && (ch = fgetc(stdin)) != '\n'; ++i)
            {
                message[i] = ch;
            }
            message[i] = (char) 0;

            time_t tm;
            tm = time(NULL);

            char result[5000];

            sprintf(result, "%s \t\t%s-> %s\n", 
				name, 
				ctime(&tm), 
				message);

            if(mq_send(server, result, strlen(result) + 1, 0) < 0)
            {
                perror("mq_send()");
                working = 0;
            }
        }
    }

    // char queue_name[100];
    // sprintf(queue_name, "/%s_queue", name);
    // mq_unlink(queue_name);
    // mq_close(server);

    return 0;
}

void signalHandler(int signal)
{	
    working = 0;
	printf("Exiting\n");	
    kill(child, SIGINT);
    exit(EXIT_SUCCESS);
}



void 
addClient()
{
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 3000;
    attr.mq_flags = O_NONBLOCK;

    char queue_name[110];
    sprintf(queue_name, "/%squeue", name);
    int perm = S_IRUSR | S_IWUSR;
    int flags = O_RDONLY | O_CREAT;

    if((client = mq_open(queue_name, flags, perm, &attr)) == -1)
    	perror("mq_open");

    char message[1000];
    sprintf(message, "!!new %s", name);
    if(mq_send(server, message, strlen(message) + 1, 0) < 0)
    	perror("mq_send");

}
void 
before_exit()
{
    char queue_name[100];
    sprintf(queue_name, "/%s_queue", name);
    mq_unlink(queue_name);
    mq_close(server);
}