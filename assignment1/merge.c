#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "w");
	FILE *nfp = fopen(argv[2], "r");
	FILE *nnfp = fopen(argv[3], "r");
	char *buffer1;
	char *buffer2;
	int size;
	
	fseek(nfp, 0, SEEK_END);
	size = ftell(nfp);
	buffer1 = malloc(size+1);
	memset(buffer1, 0, size+1);
	fseek(nfp, 0, SEEK_SET);
	if(feof(nfp) == 0)
	{
		fread(buffer1, sizeof(char), size, nfp);
		fwrite(buffer1, sizeof(char), size, fp);
	}

	fseek(nnfp, 0, SEEK_END);
	size = ftell(nnfp);
	buffer2 = malloc(size+1);
	memset(buffer2, 0, size+1);
	fseek(nnfp, 0, SEEK_SET);
	if(feof(nnfp) == 0)
	{
		fread(buffer2, sizeof(char), size, nnfp);
		fwrite(buffer2, sizeof(char), size, fp);
	}
	
	fclose(fp);
	fclose(nfp);
	fclose(nnfp);
	free(buffer1);
	free(buffer2);
	return 0;
}
