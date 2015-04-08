#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	printf("Hello from process with pid = %d\n", getpid());
}