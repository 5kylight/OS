#include <stdlib.h>
#include <stdio.h>
#include  <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[]){
	pid_t pid;
	// if((pid = fork()) < 0 ){
	// 	printf("Can't fork\n" );
	// }
	// else if(pid == 0 ){
	// 	printf("Hello from child , pid = %d\n", (int)getpid());
	// } else{
	// 	printf("Hello from parent, pid = %d\n", (int)getpid());
	// }


	for(int i = 0; i < atoi(argv[1]); i++){
		int status;
		if((pid = fork()) < 0 ){
			printf("Can't fork\n");
		}
		if(pid == 0){ 
			printf("(%d)Hello from child, pid = %d\n",i, (int) getpid());
			return 0;
		} else {
			printf("Hello from parent, pid = %d\n", (int)getpid());
			// waitpid(pid, &status,0);
		}
	}

		
}