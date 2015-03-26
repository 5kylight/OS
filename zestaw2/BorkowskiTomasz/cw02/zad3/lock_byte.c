#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int set_lock_read(int);
void menu_set_lock_read();
int set_lock_write(int);
void menu_set_lock_write();
void print_locked();
int delete_lock();
void menu_delete_lock();
int read_byte();
void menu_read_byte();
int change_byte();
void menu_change_byte();
int print_menu();

int file_d = 0;

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Usage: %s\n filename",argv[0] );
		return 1;
	}
	char *filename = argv[1];
	file_d = open(filename, O_RDWR);

   	if(file_d < 0 ){
		printf("Can't open file for writting\n");
		return 1;
	}
	
	int loop = 1;
	while(loop){
		
		switch(print_menu()){
			case 1:
				menu_set_lock_read();
				break;
			case 2:
				menu_set_lock_write();
				break;
			case 3:
				print_locked();
				break;
			case 4:
				menu_delete_lock();
				break;
			case 5:
				read_byte();
				break;
			case 6:
				change_byte();
				break;
			default:
				loop = 0;
		}
	}

	close(file_d);
	return 0;
}


void menu_set_lock_read(){
	int byte;

	printf("Which byte do you want lock for reading: \n");
	scanf("%d",&byte);
	
	if(!set_lock_read(byte))
		printf("Locked!\n");
}

int set_lock_read(int byte){

	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_start = byte;
	lock.l_whence = SEEK_SET;
	lock.l_len = 1;
	lock.l_pid = getpid();
	
	if(fcntl(file_d, F_SETLK, &lock) < 0 ){
		printf("Can't resolve lock!\n");
		return 1;
	}
	return 0;
	
}

void menu_set_lock_write(){
	int byte;
	printf("Which byte do you want lock for writing: \n");
	scanf("%d",&byte);

	if(!set_lock_write(byte))
		printf("Locked!\n");

}
int set_lock_write(int byte){
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = byte;
	lock.l_whence = SEEK_SET;
	lock.l_len = 1;
	lock.l_pid = getpid();
	
	if(fcntl(file_d, F_SETLK, &lock) == -1 ){
		perror("Can't resolve lock!");
		return 1;
	}
	return 0;
}
void print_locked(){
	int n, i;

	if((n = lseek(file_d, 0, SEEK_END)) == -1 )
		perror("Can't reach end of file!");

	char *type;

	struct flock lock;
	for(i = 0 ; i < n ; i++){

		lock.l_type = F_WRLCK;	
		lock.l_start = i;	
		lock.l_whence = SEEK_SET;
		lock.l_len = 1;

		if(fcntl(file_d, F_GETLK, &lock) == -1 ){
			perror("Can't resolve getlock!");
			return;
		}
		
		if(lock.l_type != F_UNLCK){
			if(lock.l_type == F_WRLCK)
				type = "Write";
			else if(lock.l_type == F_RDLCK)
				type = "Read ";
			printf("%s lock on byte %d by pid = %d \n", type, i, (int) lock.l_pid);

		}
	}
}
int delete_lock(int byte){
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = byte;
	lock.l_len = 1; 
	lock.l_whence = SEEK_SET;

	if(fcntl(file_d,F_SETLK,&lock) == -1){
        return 1;
	}		
	return 0;
}
void menu_delete_lock(){
	int byte;
	printf("Which lcok do you want to delete: \n");
	scanf("%d",&byte);
	if(delete_lock(byte)){
		printf("Can't unlock this one!\n");
	}


}
int read_byte(){
	int byte;
	char c;
	printf("Which byte do you want read: \n");
	scanf("%d",&byte);
	
	if(set_lock_read(byte)){
		printf("Can't open for reading\n");
		return 1;
	}

	if(lseek(file_d,byte,SEEK_SET) < 0){
		printf("Can't read this byte\n");
		return 1;
	}
	read(file_d,&c,1);
	printf("This byte is %c\n",c);

	delete_lock(byte);
	
	return 0;
}
int change_byte(){
	int byte;
	char c;
	printf("Which byte do you want change: \n");
	scanf("%d",&byte);

	if(set_lock_write(byte)){
		printf("Can't open for reading\n");
		return 1;
	}


	if(lseek(file_d,byte,SEEK_SET) < 0){
		printf("Can't read this byte\n");
		return 1;
	}
	
	read(file_d,&c,1);
	
	if(lseek(file_d,byte,SEEK_SET) < 0){
		printf("Can't read this byte\n");
		return 1;
	}
	
	printf("This byte is %c\n On what do you want change it? \n",c);
	scanf(" %s",&c);

	write(file_d,&c,1);

	delete_lock(byte);

	return 0;

}

int print_menu(){

	int response = 0; 
	printf("####################################\n");
	printf("What want you to do? Enter number\n");
	printf("1 : Set lock for read on byte\n");
	printf("2 : Set lock for write on byte\n");
	printf("3 : Print locked bytes in file\n");
	printf("4 : Delete lock on byte\n");
	printf("5 : Read byte\n");
	printf("6 : Change byte\n");
	printf("Any : Exit \n >> ");

	scanf(" %d",&response);
	printf("####################################\n");
	
	return response;
}