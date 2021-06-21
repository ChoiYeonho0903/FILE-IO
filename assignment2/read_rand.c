#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#define SUFFLE_NUM	10000	// random으로 read_order_list를 섞는다

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1], "rb");

	int *header=malloc(sizeof(int));
	char *buffer=malloc(250);
	struct timeval starttime, endtime;
	int elapsed_time;
	int *read_order_list;
	int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수
	int i=1;

	srand(time(NULL));

	
	fread(header, sizeof(header), 1, fp); //fp의 offset 4만큼 이동
	num_of_records = *header;
	read_order_list = (int*)malloc(num_of_records*sizeof(int));


	GenRecordSequence(read_order_list, num_of_records); //read_order_list 배열에 읽어야 할 레코드 번호들이 나열되어 저장

	gettimeofday(&starttime, NULL);
	fseek(fp, 250*(read_order_list[0]), SEEK_CUR); //read_order_list[0]에 들어있는 record 번호가 있는 offset으로 이동
	while(i<num_of_records)
	{
		fseek(fp, 250*(read_order_list[i]-read_order_list[i-1]), SEEK_CUR);
		fread(buffer, 250, 1, fp);
		i++;
	}
	gettimeofday(&endtime, NULL);
	elapsed_time = ((endtime.tv_sec - starttime.tv_sec)*1000000 + (endtime.tv_usec - starttime.tv_usec));
	fprintf(stdout, "#records: %d elapsed_time: %d us\n",num_of_records ,elapsed_time);
	//
	// 'read_order_list'를 이용하여 random 하게 read 할 때 걸리는 전체 시간을 측정하는 코드 구현
	//
	free(read_order_list);
	free(buffer);
	free(header);
	fclose(fp);
	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}

	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
