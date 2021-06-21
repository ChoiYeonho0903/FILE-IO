#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char *argv[]){
	FILE *fp = fopen(argv[1], "rb");

	int *header=malloc(sizeof(int));
	char *buffer=malloc(250);
	int num_of_records;
	int i=0;
	struct timeval starttime, endtime;
	int elapsed_time;
	fread(header, sizeof(header), 1, fp); //fp의 offset 4만큼 이동
	num_of_records = *header;

	gettimeofday(&starttime, NULL);
	while(i<num_of_records)
	{
		fread(buffer, 250, 1, fp);
		i++;
	}
	gettimeofday(&endtime, NULL);
	elapsed_time = ((endtime.tv_sec - starttime.tv_sec)*1000000 + (endtime.tv_usec - starttime.tv_usec));
	fprintf(stdout, "#records: %d elapsed_time: %d us\n", num_of_records, elapsed_time);

	free(buffer);
	free(header);
	fclose(fp);
	return 0;
}
