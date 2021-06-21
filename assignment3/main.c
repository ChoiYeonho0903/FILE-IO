#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blockmap.h"

FILE* flashfp;

/****************  prototypes ****************/
void ftl_open();
void ftl_write(int lsn, char* sectorbuf);
void ftl_read(int lsn, char* sectorbuf);
void ftl_print();

//
// 이 함수는 file system의 역할을 수행한다고 생각하면 되고,
// file system이 flash memory로부터 512B씩 데이터를 저장하거나 데이터를 읽어 오기 위해서는
// 각자 구현한 FTL의 ftl_write()와 ftl_read()를 호출하면 됨
//
int exists(const char* fname) {
	FILE* file;
	if ((file = fopen(fname, "r"))) {
		fclose(file);
		return 1;
	}
	else {
		return 0;
	}
}

int main(int argc, char* argv[])
{
	char* blockbuf;
	char sectorbuf[SECTOR_SIZE]="Helloworld";
	int lsn, i;

	//flashmemory 파일존재하는지 검사
	if (exists("flashmemory") == 0) {
		flashfp = fopen("flashmemory", "w+b");
		if (flashfp == NULL)
		{
			printf("file open error\n");
			exit(1);
		}

		// flash memory의 모든 바이트를 '0xff'로 초기화한다.
		blockbuf = (char*)malloc(BLOCK_SIZE);
		memset(blockbuf, 0xFF, BLOCK_SIZE);

		for (i = 0; i < BLOCKS_PER_DEVICE; i++)
		{
			fwrite(blockbuf, BLOCK_SIZE, 1, flashfp);
		}
		free(blockbuf);
	}
	else {
		flashfp = fopen("flashmemory", "r+b");
		if (flashfp == NULL)
		{
			printf("file open error\n");
			exit(1);
		}
	}
	ftl_open();
	ftl_write(0, sectorbuf);
	// ftl_write() 및 ftl_read() 테스트 코드 작성
	//
	ftl_print();
	fclose(flashfp);

	return 0;
}
