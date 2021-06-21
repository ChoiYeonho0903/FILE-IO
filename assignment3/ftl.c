// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"

// 필요한 경우 헤더 파일을 추가하시오.

extern int dd_read(int ppn, char* pagebuf);
extern int dd_write(int ppn, char* pagebuf);
extern int dd_erase(int pbn);

typedef struct {
	int lbn;
	int pbn;
}Mapping_table;

Mapping_table FTL[DATABLKS_PER_DEVICE];
int FB_pbn;
int ret;

int binaryToInt(char* s) {
	int i = 0;
	for (int j = 0; j < 4; j++) {
		i = (i << 8) | s[j];
	}
	if (i == -1)
		return i;
	else {
		i = strtol(s, NULL, 16);
		return i;
	}
}

// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.

void ftl_open()
{
	// address mapping table 초기화 또는 복구
	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	for (int i = 0; i < DATABLKS_PER_DEVICE; i++) {
		FTL[i].lbn = i;
		FTL[i].pbn = -1;
	}
	FB_pbn = 15;
	// free block's pbn 초기화

	char* pagebuf=(char*)malloc(PAGE_SIZE);
	char* lbnbuf=(char*)malloc(4);
	int lbn;
	for (int i = 0; i < BLOCKS_PER_DEVICE; i++)
	{
		ret = dd_read(i*4, pagebuf);
		if (ret != 1) {
			printf("read error\n");
		}
		memcpy(lbnbuf, pagebuf + SECTOR_SIZE, 4);
		lbn = binaryToInt(lbnbuf);
		if (lbn >= 0) {
			FTL[lbn].pbn = i;
		}
		else if (lbn == -1 && FB_pbn > i) {
			FB_pbn = i;
		}
	}
	free(pagebuf);
	free(lbnbuf);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char* sectorbuf)
{
	char* pagebuf = (char*)malloc(PAGE_SIZE);
	int lbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int ret = dd_read(FTL[lbn].pbn * 4 + offset, pagebuf);
	if (ret < 0)
		printf("read error\n");
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char* sectorbuf)
{
	char* pagebuf = malloc(PAGE_SIZE);
	memset(pagebuf, 0, PAGE_SIZE);
	char* pagebuf_RW = malloc(PAGE_SIZE);
	memset(pagebuf_RW, 0, PAGE_SIZE);
	char* lbnbuf = malloc(4);
	memset(lbnbuf, 0, 4);
	char* lsnbuf = malloc(4);
	memset(lsnbuf, 0, 4);
	int lbn = lsn / PAGES_PER_BLOCK;
	int offset = lsn % PAGES_PER_BLOCK;
	int FM_lsn;

	sprintf(lbnbuf, "%d", lbn);
	sprintf(lsnbuf, "%d", lsn);
	memcpy(pagebuf, sectorbuf, 512);
	memcpy(pagebuf + SECTOR_SIZE, lbnbuf, 4);
	memcpy(pagebuf + SECTOR_SIZE + 4, lsnbuf, 4);

	//최초의 데이터를 쓸 때 
	if (FTL[lbn].pbn == -1) {
		FTL[lbn].pbn = lbn;
		dd_write(FTL[lbn].pbn * 4 + offset, pagebuf);	
		if (offset != 0) {   //블록 첫번째 페이지가 아닐 때
			memset(pagebuf_RW, -1, PAGE_SIZE);
			memcpy(pagebuf_RW + SECTOR_SIZE, lbnbuf, 4);
			dd_write(FTL[lbn].pbn * 4, pagebuf_RW);
		}
	}
	//pbn값이 있을 때
	else {
		ret = dd_read(FTL[lbn].pbn*4+offset, pagebuf_RW);
		if (ret != 1) {
			printf("read error\n");
		}
		memcpy(lsnbuf, pagebuf_RW + SECTOR_SIZE + 4, 4);
		FM_lsn = binaryToInt(lsnbuf);
		//flashmeomory 블락의 해당 페이지의 spare위치에 lsn 값이 없다.
		if (FM_lsn == -1) {
			dd_write(FTL[lbn].pbn * 4 + offset, pagebuf);
		}
		//flashmemory 블락의 해당 페이지의 spare위치에 lsn 값이 있다.=> 갱신
		else {
			//pbn에 있는 나머지 data freeblock으로 옮기기
			for (int i = 0; i < 3; i++) {
				if (i != offset) {
					dd_read(FTL[lbn].pbn * 4 + i, pagebuf_RW);
					dd_write(FB_pbn * 4 + i, pagebuf_RW);
				}
			}
			dd_write(FB_pbn * 4 + offset, pagebuf);
			int temp;
			temp = FTL[lbn].pbn;
			FTL[lbn].pbn = FB_pbn;
			FB_pbn = temp;
			dd_erase(FB_pbn);
		}
	}
	free(pagebuf);
	free(lbnbuf);
	free(lsnbuf);
	free(pagebuf_RW);
	return;
}

void ftl_print()
{
	printf("lbn pbn\n");
	for (int i = 0; i < (BLOCKS_PER_DEVICE-1); i++) {
		printf("%d %d\n", FTL[i].lbn, FTL[i].pbn);
	}
	printf("free block's pbn=%d\n", FB_pbn);
	return;
}
