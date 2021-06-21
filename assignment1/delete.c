#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "r+");
	int offset=atoi(argv[2]);
	int del=atoi(argv[3]);
	char *buffer;
	int size;
	int buffersize;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	if(ftell(fp)>=(offset+del))
	{
		buffersize = size-offset-del;
	}
	else
	{
		buffersize = del;
	}
	buffer = malloc(size+1);
	memset(buffer, 0, size+1);
	
	fseek(fp, offset+del, SEEK_SET);
	fread(buffer, sizeof(char), buffersize, fp);

	fseek(fp, offset, SEEK_SET); 
	fwrite(buffer, sizeof(char), buffersize, fp);
	
	fflush(fp);
	ftruncate(fileno(fp), size-del); 

	fclose(fp);
	free(buffer);
	return 0;
}
