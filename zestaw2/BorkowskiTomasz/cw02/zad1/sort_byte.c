#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


void bubble_sort_lib(char *, size_t);
void bubble_sort_sys(char *, size_t);

void print_time_info(clock_t, struct tms *, struct tms *);


int main(int argc, char *argv[]){

	if(argc < 3){
		printf("Too few arguments. Usage: %s input_filename record_size \n", argv[0] );
		return 1 ; 
	}
	char *name = (char*) malloc(sizeof(char) * strlen(argv[1]) + 1);
	strcpy(name,argv[1]);
   	strcat(name,"1");

	bubble_sort_lib(argv[1], atoi(argv[2]));
	bubble_sort_sys(name, atoi(argv[2]));

}


void bubble_sort_lib(char *filename, size_t record_size){
	
	/* Prepare to  measuring time */
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");
   
   	/*Opening file */
   	FILE *file = fopen(filename, "r+");

   	if(file == NULL ){
		printf("There is no file with that name! \n");
		return;
	}
		
   	char *buff1 = (char*)malloc(sizeof(char)*record_size);
   	char *buff2 = (char*)malloc(sizeof(char)*record_size);

   	// Counting elements and copy file
   	int w;
   	int n = 0 ;
   	char *copy_filename = (char*) malloc(sizeof(char)*(strlen(filename)+5));
   	strcpy(copy_filename,filename);
   	strcat(copy_filename,".copy");
   	FILE *copy_file = fopen(copy_filename, "w");

   	if(copy_file == NULL ){
		printf("Can't open file for write copy! \n");
		return;
	}


	while(fread(buff1,sizeof(char),record_size,file) > 0 ){
		++n;
		fwrite(buff1, sizeof(char),record_size,copy_file);
	}
	w = n;
	fclose(copy_file);
	fseek(file, 0, 0);

	while(n > 0 ){
		while( fread(buff1,sizeof(char),record_size,file) > 0 
		&& fread(buff2,sizeof(char),record_size,file) > 0){
			fseek(file,(-2*record_size),1);

			if(buff1[0] < buff2[0]){
				fwrite(buff1, sizeof(char),record_size,file);
				fwrite(buff2, sizeof(char),record_size,file);
			} else{
				fwrite(buff2, sizeof(char),record_size,file);
				fwrite(buff1, sizeof(char),record_size,file);
			}
		fseek(file,(-1*record_size),1);
		}

	fseek(file,0,0);
	n--;
   }
   fclose(file);

   	/* Finish and print time info: */
   	if((stop_time = times(&stop_tms)) == -1)
		printf("Time error\n");
   	

	printf("Bubble sorting %d-file using library funs with %d record size took: \n",w,(int) record_size);
	print_time_info(stop_time - start_time, &start_tms, &stop_tms);
}

void bubble_sort_sys(char *filename, size_t record_size){
	/* Prepare to  measuring time */
	struct tms start_tms, stop_tms;
   	clock_t start_time, stop_time;
   	if((start_time = times(&start_tms)) == -1)
   		printf("Time error: \n");
   	

   	/*Opening file */
   	int file = open(filename, O_RDWR);

   	if(file < 0 ){
		printf("Can't open file for writting\n");
		return;
	}
		
   	
   	/*Sorting */
   	char *buff1 = (char*)malloc(sizeof(char)*record_size);
   	char *buff2 = (char*)malloc(sizeof(char)*record_size);

   	// Counting elements and copy file
   	char *copy_filename = (char*) malloc(sizeof(char)*(strlen(filename)+5));
   	strcpy(copy_filename,filename);
   	strcat(copy_filename,".copy");
   	int copy_file = open(copy_filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IRGRP  );

   	if(copy_file < 0 ){
		printf("Can't open file for write copy!\n");
		return;
	}

   	int n = 0 ;
	while(read(file, buff1, record_size) > 0) {
		++n;
		write(copy_file, buff1, record_size);
	}
	int w = n;

	close(copy_file);
	lseek(file,0,SEEK_SET);

	// while(n > 0 && flag){
	while(n > 0 ){
		
		// flag = 0;
		while( read(file, buff1, record_size) > 0 
		&& read(file, buff2, record_size) > 0){

			lseek(file,(-2*record_size),SEEK_CUR);
			if(buff1[0] < buff2[0]){
				write(file, buff1,record_size);
				write(file, buff2,record_size);
			} else{
				write(file, buff2,record_size);
				write(file, buff1,record_size);
			}
		lseek(file,(-1*record_size),SEEK_CUR);
		}

	lseek(file,0,SEEK_SET);
	n--;
   }
   close(file);

   	/* Finish and print time info: */
   	if((stop_time = times(&stop_tms)) == -1)
		printf("Time error\n");
   	

	printf("Bubble sorting %d-file using system funs with %d record size took: \n",w,(int) record_size);
	print_time_info(stop_time - start_time, &start_tms, &stop_tms);

}

void print_time_info(clock_t clock, struct tms *start_time, struct tms *current_time)
{

	long ticks = sysconf(_SC_CLK_TCK);
	printf("\t\tReal time is:  \t%.2f\n", clock/ (double) ticks);
	printf("\t\tUser time is:  \t%.2f\n",( current_time->tms_utime - start_time->tms_utime )/(double) ticks );
	printf("\t\tSystem time is:\t%.2f\n",( current_time->tms_stime - start_time->tms_stime )/(double) ticks );

}