#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "r");
	int offset=atoi(argv[2]);
	int rd=atoi(argv[3]);
	char *buffer;

	buffer = malloc(rd+1);	
	memset(buffer, 0, rd+1); 

	fseek(fp, offset, SEEK_SET);
	fread(buffer, sizeof(char), rd, fp);

	printf("%s\n", buffer);

	fclose(fp);
	free(buffer);
	return 0;
}


