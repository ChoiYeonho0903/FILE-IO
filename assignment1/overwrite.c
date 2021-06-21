#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "r+");
	int offset=atoi(argv[2]);
	
	fseek(fp, offset, SEEK_SET);
	for(int i=3; i<argc; i++)
	{
		fwrite(argv[i], strlen(argv[i]), 1, fp);
	}

	fclose(fp);
	return 0 ;
}
