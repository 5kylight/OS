#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <time.h>

int permiss = 0;

int give_permission(int n,  struct stat);
void visit_dir(char *, int);
int visit_ftw(const char*, const struct stat*, int);

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("Too few arguments. Usage: %s start_directory permissions \n", argv[0] );
		return 1 ; 
	}
	if(strlen(argv[2])!=3){
		printf("Bad permissions type, it must be like: 777 or 421 \n" );
	}


	int per = permiss = atoi(argv[2]);
	char *path = argv[1]; 

	visit_dir(path,per);
	// sruct stat *ptr;

	// ftw(path, visit_ftw,16);
}

 
int give_permission(int n, struct stat buf){  // give informations, if 7 have permissons to read etc
	//returns 0 if have permission, else 1	
	int u,g,o;
	u = g = o = 0;

	int per = 0;	


	// recovering rights from st_mode
	if( buf.st_mode & S_IRUSR  )
		u+=4;
	if( buf.st_mode & S_IWUSR )
		u+=2;
	if( buf.st_mode & S_IXUSR )
		u+=1;
	if( buf.st_mode & S_IRGRP )
		g+=4;
	if(buf.st_mode & S_IWGRP  )
		g+=2;
	if( buf.st_mode & S_IXGRP )
		g+=1;
	if ( buf.st_mode & S_IROTH )
		o+=4;
	if( buf.st_mode & S_IWOTH )
		o+=2;
	if(  buf.st_mode & S_IXOTH)
		o+=1;

	per+=u;
	per=10*per;
	per+=g;
	per=10*per;
	per+=o;

	if(per == n)
		return 0;


	return 1;
}

void visit_dir(char *path, int permissions){
	DIR *dir = opendir(path);
	if(!dir){
		printf("Can't open dir: %s\n",path );
		return;	
	}
	struct dirent *current;

	struct stat info;
	char *full_path;
	while((current = readdir(dir)) != NULL ){
		full_path = (char*)malloc(sizeof(char)*(2+strlen(path)+strlen(current -> d_name)));
		if(strcmp(current->d_name, ".") == 0 || strcmp(current->d_name, "..")== 0  )
			continue;

		strcpy(full_path, path);
		strcat(full_path,"/");
		strcat(full_path, current -> d_name);

		if(lstat(full_path, &info)>=0){
			if(S_ISREG(info.st_mode)){
				if(give_permission(permissions, info) == 0){
					printf("\t%s \t\t | size: %d \t\t | last edited: %s  ",full_path   ,(int) info.st_size, ctime(&info.st_mtim.tv_sec));
				}
			} else if(S_ISDIR(info.st_mode)){
				visit_dir(full_path, permissions);
			}
		} else {

			// printf("%s Can't open file\n", full_path);
		}
		free(full_path);
	}
	closedir(dir);
	return;

}


int visit_ftw(const char *path, const struct stat *ptr, int type){
	if(type == FTW_F && give_permission(permiss, *ptr ) == 0){
			printf("%s  size: %d   \n",path,(int) ptr -> st_size );
	}
	return 0;
}


