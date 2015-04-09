#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

int p = 0;

int main(int argc, char *argv[]){

	/* Checking arugment number*/
	if(argc < 2){
		printf("Usage %s <start_path> \n",argv[0] );
		return 1;
	}

	/* Looking for path and -w flag*/
	char *path;
	int w_flag = 0;

	for(int i = 1; i < argc ; i++){
		if( strcmp(argv[i],"-w") == 0 ){
			w_flag = 1	;
		} else if(strlen(argv[i]) > 1) {
			path = argv[i];
		}
	}
	
	/* Try to open dir, if impossible print error and return */
	DIR *dir = opendir(path);
	if(dir == NULL){
		printf("\tCan't open file: %s\n", path);
		exit(-8);
	}
	struct dirent *current;
	struct stat info;
	char *full_path;
	pid_t pid;

	int file_counter = 0;
	int counter = 0;

	/* Reading directory */
	while((current = readdir(dir)) != NULL){
		
		/* Alloc place for new path */			
		full_path = (char*) malloc(sizeof(char)*(2 + strlen(path) + strlen(current -> d_name)));
		
		/* Skiping current folder and folder above in tree */
		if(strcmp(current->d_name, ".") == 0 
				|| strcmp(current->d_name, "..")== 0)
			continue;

		strcpy(full_path, path);
		strcat(full_path, "/");
		strcat(full_path, current -> d_name);

		if(lstat(full_path, &info) >= 0){

			/* Checing if this is regular file */
			if(S_ISREG(info.st_mode)){
				file_counter++;				

			/* If directory */
			} else if(S_ISDIR(info.st_mode)){

				if((pid = fork()) >= 0 ){
					if( pid == 0 ){
						int returned = 0; 
						if(w_flag)
							returned = (execl(argv[0], argv[0], full_path, "-w",";", (char*)NULL));
						else
							returned = (execl(argv[0], argv[0], full_path, ";", (char*)NULL));
						exit(returned);
					} else
						counter++;		
				} else {
					printf("Forking error\n");
					exit(-3);
				}
			}
		} else 
			printf("\t Can't open file: %s \n", full_path);
		
		free(full_path);
	}

	closedir(dir);

	/* Waiting for all process, and sum their returned value */
	while(counter > 0){
		int status, val;				
		wait(&status);
		if(WIFEXITED(status)){
			val = WEXITSTATUS(status);
			if(val < 0){
				printf("Forked process exit with value %d\n", val );
				exit(-2);	
			} else{
			file_counter += val;
			}
		counter--;
		}

	}

	/* If flag was setted wait for small period of time */
	if(w_flag)
		sleep(3);

	/* If this was launched by console exit(0) and print result */
	if(argc <= 2 || (w_flag && argc <= 3) ){
		printf("Total files number in %s is %d\n",path, file_counter);
		exit(0);
	} 

	/* If this prog was runned by exec return number of files in this directory */
	exit(file_counter);
}	
