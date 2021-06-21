#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE* fp, char* pagebuf, int pagenum)
{
	fseek(fp, 16 + pagenum * PAGE_SIZE, SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE* fp, const char* pagebuf, int pagenum)
{
	fseek(fp, 16 + pagenum * PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}
//인덱스 읽기
void readiRecord(FILE* idxfp, char* recordbuf, int recordnum)
{
	fseek(idxfp, 4 + recordnum * 21, SEEK_SET);
	fread(recordbuf, 21, 1, idxfp);
}
//인덱스 쓰기
void writeiRecord(FILE* idxfp, const char* recordbuf, int recordnum)
{
	fseek(idxfp, 4 + recordnum * 21, SEEK_SET);
	fwrite(recordbuf, 21, 1, idxfp);
}
//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char* recordbuf, const Person* p)
{

}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char* recordbuf, Person* p)
{
	char getChar;
	char word[50];
	int wordlen;
	int j = 0;
	for (int i = 0; i < 6; i++) {
		wordlen = 0;
		memset(word, 0, sizeof(word));
		while (1) {
			if (memcpy(&getChar, recordbuf + j, 1)) {
				if (getChar == '#') {
					j++;
					break;
				}
				else {
					j++;
					word[wordlen++] = getChar;
				}
			}
			else {
				printf("Error to read");
				break;
			}
		}

		if (i == 0) {
			strcpy(p->id, word);
		}
		else if (i == 1) {
			strcpy(p->name, word);
		}
		else if (i == 2) {
			strcpy(p->age, word);
		}
		else if (i == 3) {
			strcpy(p->addr, word);
		}
		else if (i == 4) {
			strcpy(p->phone, word);
		}
		else if (i == 5) {
			strcpy(p->email, word);
		}
	}
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE* fp, const Person* p)
{

}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE* fp, const char* id)
{

}

//
// 주어진 레코드 파일(recordfp)을 이용하여 심플 인덱스 파일(idxfp)을 생성한다.
//
void createIndex(FILE* idxfp, FILE* recordfp)
{
	char* pagebuf = (char*)malloc(PAGE_SIZE);
	char* idbuf = (char*)malloc(14);
	char* keybuf = (char*)malloc(13);
	int pagenum; //전체 페이지수
	int recordnum; //전체 레코드수
	int page_record_num; //page에 저장되어 있는 레코드의 수
	int offset;
	int length;
	char ch;
	char* recordbuf = (char*)malloc(21);
	int idx_recordnum;
	long long id;
	long long key;
	int kk;
	fseek(recordfp, 0, SEEK_SET);
	fread(&pagenum, 4, 1, recordfp);
	fread(&recordnum, 4, 1, recordfp);

	for (int i = 0; i < pagenum; i++) {
		readPage(recordfp, pagebuf, i);
		memcpy(&page_record_num, pagebuf, 4);
		for (int j = 1; j <= page_record_num; j++) {
			memcpy(&offset, pagebuf + 4 + (j - 1) * 8, 4);
			memcpy(&length, pagebuf + 4 + (j - 1) * 8 + 4, 4);
			memset(idbuf, 0, 14);
			for (int k = 0; k < 14; k++) {
				memcpy(&ch, pagebuf + HEADER_AREA_SIZE + offset + k, 1);
				if (ch == '#') {
					break;
				}
				else {
					idbuf[k] = ch;
				}
			}

			//삭제레코드 제외
			if (idbuf[0] == '*')
				continue;

			id = atoll(idbuf);
			fseek(idxfp, 0, SEEK_SET);
			fread(&idx_recordnum, 4, 1, idxfp);
			if (idx_recordnum == 0) {
				memset(recordbuf, 0, 21);
				memcpy(recordbuf, idbuf, 13);
				memcpy(recordbuf + 13, &i, 4);
				memcpy(recordbuf + 17, &j, 4);
				idx_recordnum++;
				fseek(idxfp, 0, SEEK_SET);
				fwrite(&idx_recordnum, 4, 1, idxfp);
				writeiRecord(idxfp, recordbuf, 0);
			}
			else {
				for (kk = 0; kk < idx_recordnum; kk++) {
					fseek(idxfp, 4 + 21 * kk, SEEK_SET);
					fread(keybuf, 13, 1, idxfp);
					key = atoll(keybuf);
					if (id < key) {
						for (int m = idx_recordnum - 1; m >= kk; m--) {
							readiRecord(idxfp, recordbuf, m);
							writeiRecord(idxfp, recordbuf, m + 1);
						}
						memset(recordbuf, 0, 21);
						memcpy(recordbuf, idbuf, 13);
						memcpy(recordbuf + 13, &i, 4);
						memcpy(recordbuf + 17, &j, 4);
						writeiRecord(idxfp, recordbuf, kk);
						idx_recordnum++;
						fseek(idxfp, 0, SEEK_SET);
						fwrite(&idx_recordnum, 4, 1, idxfp);
						break;
					}
				}
				if (kk == idx_recordnum) {
					memset(recordbuf, 0, 21);
					memcpy(recordbuf, idbuf, 13);
					memcpy(recordbuf + 13, &i, 4);
					memcpy(recordbuf + 17, &j, 4);
					writeiRecord(idxfp, recordbuf, kk);
					idx_recordnum++;
					fseek(idxfp, 0, SEEK_SET);
					fwrite(&idx_recordnum, 4, 1, idxfp);
				}
			}
		}
	}
}

//
// 주어진 심플 인덱스 파일(idxfp)을 이용하여 주민번호 키값과 일치하는 레코드의 주소, 즉 페이지 번호와 레코드 번호를 찾는다.
// 이때, 반드시 이진 검색 알고리즘을 사용하여야 한다.
//
void binarysearch(FILE* idxfp, const char* id, int* pageNum, int* recordNum)
{
	char* keybuf = (char*)malloc(13);
	long long key;
	long long id1;
	int idx_recordnum;
	id1 = atoll(id);
	fseek(idxfp, 0, SEEK_SET);
	fread(&idx_recordnum, 4, 1, idxfp);
	int first = 0;
	int last = idx_recordnum-1;
	int mid = 0;
	int count = 0;
	while (first <= last) {
		count++;
		mid = (first + last) / 2;
		fseek(idxfp, 4 + 21 * mid, SEEK_SET);
		fread(keybuf, 13, 1, idxfp);
		key = atoll(keybuf);
		if (key == id1) {
			break;
		}
		else {
			if (key > id1)
				last = mid - 1;
			else
				first = mid + 1;
		}
	}

	printf("#reads:%d\n", count);
	if (first > last) {
		*pageNum = -1;
		*recordNum = -1;
	}
	else {
		fseek(idxfp, 4 + 21 * mid + 13, SEEK_SET);
		fread(pageNum, 4, 1, idxfp);
		fread(recordNum, 4, 1, idxfp);
	}
}

int main(int argc, char* argv[])
{
	FILE* idxfp; // 레코드 파일의 파일 포인터
	FILE* recordfp;
	int* pageNum=malloc(sizeof(int));
	int* recordNum = malloc(sizeof(int));
	if (strcmp(argv[1], "i")==0) {
		recordfp = fopen(argv[2], "r+b");
		idxfp = fopen(argv[3], "w+b");
		int idxhead = 0;
		fwrite(&idxhead, 4, 1, idxfp);
		createIndex(idxfp, recordfp);
	}
	else if (strcmp(argv[1], "b")== 0) {
		recordfp = fopen(argv[2], "r+b");
		idxfp = fopen(argv[3], "r+b");
		binarysearch(idxfp, argv[4], pageNum, recordNum);
		if (*pageNum == -1 && *recordNum == -1)
			printf("no persons");
		else {
			char* recordbuf = (char*)malloc(MAX_RECORD_SIZE);
			memset(recordbuf, 0, MAX_RECORD_SIZE);
			char* pagebuf = (char*)malloc(PAGE_SIZE);
			memset(pagebuf, 0, PAGE_SIZE);
			Person* p = (Person*)malloc(sizeof(Person));
			int offset;
			int length;
			readPage(recordfp, pagebuf, *pageNum);
			memcpy(&offset, pagebuf + 4 + (*recordNum-1) * 8, 4);
			memcpy(&length, pagebuf + 4 + (*recordNum-1) * 8 + 4, 4);
			memcpy(recordbuf, pagebuf + HEADER_AREA_SIZE + offset, length);
			unpack(recordbuf, p);
			printf("id=%s\n", p->id);
			printf("name=%s\n", p->name);
			printf("age=%s\n", p->age);
			printf("addr=%s\n", p->addr);
			printf("phone=%s\n", p->phone);
			printf("email=%s\n", p->email);
		}
	}
	return 0;
}
