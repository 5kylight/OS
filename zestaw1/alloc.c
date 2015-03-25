#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

	int n;
	sscanf (argv[1],"%d",&n);
	float *tab  = (float*) calloc (n ,sizeof(float));
	for(int i = 0 ; i < n ; i++)
		printf("%f\n",tab[i]);
	return 0;


}