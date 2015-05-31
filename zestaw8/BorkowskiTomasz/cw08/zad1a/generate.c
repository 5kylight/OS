#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>


#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{
	if(argc < 4){
		printf("Too few arguments. Usage: %s output_file_name record_size number_of_records \n", argv[0] );
		return 1 ; 
	}

	int record_size = atoi(argv[2]);
	int number_of_records = atoi(argv[3]);
	char *name;

	name = argv[1];
	char buf;
	srand(time(NULL));

	// C librarys way
	FILE *file;
	file = fopen(name,"w");
	if(file == NULL ){
		printf("Can't open file for writing, \n");
		return 1;
	}
	for(int i = 0; i < number_of_records; i++){
		fprintf(file, "%d ", i);
		char i_str[10];
		sprintf(i_str, "%d", i);
		for(int j = 0; j < record_size - strlen(i_str) - 2; j++ ){
			buf = '0' + (rand()%60);
			fwrite(&buf, 1, sizeof(buf), file);			
		}
		fwrite("\n", sizeof(char), 1, file);
	}
	fclose(file);


	// To do only library things 
	return 0;	

	// System funs way
	char *newname = (char*)malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newname,name);
	newname[strlen(newname)]= '1';
	

	int file_d = open(newname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP  );

    if(file_d < 0){
    	printf("Can't open or create file\n");
    	return 1;
    }

 	for(int i = 0 ; i < number_of_records ; i++){
		for(int j = 0 ; j < record_size ; j++ ){
			buf = '0' + (rand()%60);
			write(file_d,&buf,1);
		}
		write(file_d, "\n", 2);			
	}
}
