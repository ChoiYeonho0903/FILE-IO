#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "r+");
	int offset=atoi(argv[2]);
	char *buffer;
	int size;
	int buffersize;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	buffersize = size-offset;
	buffer = malloc(buffersize+1);
	memset(buffer, 0, buffersize+1);

	fseek(fp, offset, SEEK_SET);
	fread(buffer, sizeof(char), buffersize, fp);

	fseek(fp, offset, SEEK_SET);
	for(int i=3; i<argc; i++)
	{
		fwrite(argv[i], strlen(argv[i]), 1, fp);
	}
	fwrite(buffer, sizeof(char), buffersize, fp);

	fclose(fp);
	free(buffer);
	return 0;
}
