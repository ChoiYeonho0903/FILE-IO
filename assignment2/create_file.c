#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
 
int main(int argc, char *argv[])
{
	FILE* fp = fopen(argv[2], "wb");
	int num = atoi(argv[1]);
	int i=0;
	char *buffer=malloc(250);
	int *header=malloc(4);
	memset(header, 0, sizeof(header));
	memset(buffer, 'a', 250);
	*header=num;
	
	fwrite(header,4,1,fp);
	while (i<num)
	{
		fwrite(buffer, 250, 1, fp);
		i++;
	}

	free(buffer);
	free(header);
	fclose(fp);
	return 0;
}
