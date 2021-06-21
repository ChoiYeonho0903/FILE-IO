#include <stdio.h>
#include <string.h>
#include "blockmap.h"

extern FILE *flashfp;

int dd_read(int ppn, char *pagebuf)
{
	int ret;

	fseek(flashfp, PAGE_SIZE*ppn, SEEK_SET);
	ret = fread((void *)pagebuf, PAGE_SIZE, 1, flashfp);
	if(ret == 1) {
		return 1;
	}
	else {
		return -1;
	}
}

int dd_write(int ppn, char *pagebuf)
{
	int ret;

	fseek(flashfp, PAGE_SIZE*ppn, SEEK_SET);
	ret = fwrite((void *)pagebuf, PAGE_SIZE, 1, flashfp);
	if(ret == 1) {			
		return 1;
	}
	else {
		return -1;
	}
}

int dd_erase(int pbn)
{
	char blockbuf[BLOCK_SIZE];
	int ret;

	memset((void*)blockbuf, (char)0xFF, BLOCK_SIZE);
	
	fseek(flashfp, BLOCK_SIZE*pbn, SEEK_SET);
	
	ret = fwrite((void *)blockbuf, BLOCK_SIZE, 1, flashfp);
	
	if(ret == 1) { 
		return 1;
	}
	else {
		return -1;
	}
}
