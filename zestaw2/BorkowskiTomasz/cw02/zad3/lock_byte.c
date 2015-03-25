#include <stdlib.h>
#include <stdio.h>

void set_lock_read();
void menu_set_lock_read();
void set_lock_write();
void menu_set_lock_write();
void print_locked();
void delete_lock();
void menu_delete_lock();
void read_byte();
void menu_read_byte();
void change_byte();
void menu_change_byte();
int print_menu();

int file_d;

int main(){
	int loop = 1;

	while(loop){
		system("clear");
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
				menu_read_byte();
				break;
			case 6:
				menu_change_byte();
				break;
			default:
				loop = 0;
		}
	}
}
int print_menu(){
	int response = 0; 

	printf("####################################\n");
	printf("What want you to do? Enter number:\n");
	printf("1 : Set lock for read on byte:\n");
	printf("2 : Set lock for write on byte:\n");
	printf("3 : Print locked bytes in file:\n");
	printf("4 : Delete lock on byte:\n");
	printf("5 : Read byte:\n");
	printf("6 : Change byte:\n");
	printf("Any : Exit \n");

	scanf("%d",&response);

	return response;
}


void set_lock_read(){

}
void menu_set_lock_read(){

}
void set_lock_write(){

}
void menu_set_lock_write(){

}
void print_locked(){

}
void delete_lock(){

}
void menu_delete_lock(){

}
void read_byte(){

}
void menu_read_byte(){

}
void change_byte(){

}
void menu_change_byte(){

}
