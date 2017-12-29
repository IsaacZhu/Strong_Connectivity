#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 256
int main(){
	int i,arcnum;
	arcnum = N*(int)log2(N);
	int arcfrom[arcnum+1],arcto[arcnum+1];
	for (i=1;i<=arcnum;++i){
		arcfrom[i] = (int)rand()%N+1;
		arcto[i] = (int)rand()%N+1;
	}

	FILE *fp;
	fp=fopen("Input/input.txt","w+");
	srand(time(0));
	printf("Making some random numbers...\n");
	for (i=1;i<=arcnum;++i){
		fprintf(fp,"%d,%d\n",arcfrom[i],arcto[i]);		
	}
	printf("Success!\n");
	fclose(fp);
}
