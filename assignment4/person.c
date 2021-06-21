#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음
#define HEADER_RECORD_SIZE 16 
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
	fseek(fp, HEADER_RECORD_SIZE + pagenum * PAGE_SIZE, SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE* fp, const char* pagebuf, int pagenum)
{
	fseek(fp, HEADER_RECORD_SIZE + pagenum * PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char* recordbuf, const Person* p)
{
	char *charbuf = "#";
	int offset;
	memcpy(recordbuf, p->id, strlen(p->id));
	offset = strlen(p->id);
	memcpy(recordbuf + offset, charbuf, 1);
	offset++;
	memcpy(recordbuf + offset, p->name, strlen(p->name));
	offset += strlen(p->name);
	memcpy(recordbuf + offset, charbuf, 1);
	offset++;
	memcpy(recordbuf + offset, p->age,strlen(p->age));
	offset += strlen(p->age);
	memcpy(recordbuf + offset, charbuf, 1);
	offset++;
	memcpy(recordbuf + offset, p->addr, strlen(p->addr));
	offset += strlen(p->addr);
	memcpy(recordbuf + offset, charbuf, 1);
	offset++;
	memcpy(recordbuf + offset, p->phone, strlen(p->phone));
	offset += strlen(p->phone);
	memcpy(recordbuf + offset, charbuf, 1);
	offset++;
	memcpy(recordbuf + offset, p->email, strlen(p->email));
	offset += strlen(p->email);
	memcpy(recordbuf + offset, charbuf, 1);
	offset++;
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
			if (memcpy(&getChar, recordbuf+j, 1)) {
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
	char* recordbuf=(char*)malloc(MAX_RECORD_SIZE);
	memset(recordbuf, 0, MAX_RECORD_SIZE);
	char* pagebuf = (char*)malloc(PAGE_SIZE);
	memset(pagebuf, 0, PAGE_SIZE);
	pack(recordbuf, p);
	int pagenum; //전체 페이지수
	int recordnum; //전체 레코드수
	int delete_pagenum; // 가장최근 삭제레코드 페이지 번호
	int delete_recordnum; // 가장 최근 삭제레코드 레코드 번호
	int offset;
	int length;
	int page_record_num;
	int pre_delete_pagenum;
	int new_delete_pagenum;
	int new_delete_recordnum;
	int pre_delete_recordnum;
	int count=0;
	fseek(fp, 0, SEEK_SET);
	fread(&pagenum, 4, 1, fp);
	fread(&recordnum, 4, 1, fp);
	fread(&delete_pagenum, 4, 1, fp);
	fread(&delete_recordnum, 4, 1, fp);
	while (1) {
		count++;
		//빈 파일인 경우
		if (recordnum == 0) {
			page_record_num = 1;
			offset = 0;
			length = strlen(recordbuf);
			memcpy(pagebuf, &page_record_num, 4);
			memcpy(pagebuf + 4, &offset, 4);
			memcpy(pagebuf + 8, &length, 4);
			memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
			writePage(fp, pagebuf, 0);

			//헤드 레코드 갱신
			pagenum++;
			recordnum++;
			fseek(fp, 0, SEEK_SET);
			fwrite(&pagenum, 4, 1, fp);
			fwrite(&recordnum, 4, 1, fp);

			break;
		}
		//파일 끝에 append
		else if (delete_pagenum == -1) {
			readPage(fp, pagebuf, (pagenum - 1));
			memcpy(&page_record_num, pagebuf, 4);
			memcpy(&offset, pagebuf + 4 + (page_record_num - 1) * 8, 4);
			memcpy(&length, pagebuf + 4 + (page_record_num - 1) * 8 + 4, 4);
			//새로운 page에 추가
			if ((strlen(recordbuf) + HEADER_AREA_SIZE + offset + length > PAGE_SIZE)||(page_record_num+1)*8+4>HEADER_AREA_SIZE) {
				memset(pagebuf, 0, PAGE_SIZE);
				page_record_num = 1;
				offset = 0;
				length = strlen(recordbuf);
				memcpy(pagebuf, &page_record_num, 4);
				memcpy(pagebuf + 4, &offset, 4);
				memcpy(pagebuf + 8, &length, 4);
				memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, length);
				writePage(fp, pagebuf, pagenum);

				//헤더레코드 갱신
				recordnum++;
				pagenum++;
				fseek(fp, 0, SEEK_SET);
				fwrite(&pagenum, 4, 1, fp);
				fwrite(&recordnum, 4, 1, fp);

				break;
			}
			//기존 page에 추가
			else {
				page_record_num++;
				offset = offset + length;
				length = strlen(recordbuf);
				memcpy(pagebuf, &page_record_num, 4);
				memcpy(pagebuf + (page_record_num - 1) * 8 + 4, &offset, 4);
				memcpy(pagebuf + (page_record_num - 1) * 8 + 8, &length, 4);
				memcpy(pagebuf + HEADER_AREA_SIZE + offset, recordbuf, strlen(recordbuf));
				writePage(fp, pagebuf, (pagenum - 1));

				//헤더레코드 갱신
				recordnum++;
				fseek(fp, 4, SEEK_SET);
				fwrite(&recordnum, 4, 1, fp);

				break;
			}
		}
		else {
			//삭제 레코드 공간에 추가 
			readPage(fp, pagebuf, delete_pagenum);
			memcpy(&offset, pagebuf + 4 + 8 * delete_recordnum, 4);
			memcpy(&length, pagebuf + 4 + 8 * delete_recordnum + 4, 4);
			memcpy(&new_delete_pagenum, pagebuf + HEADER_AREA_SIZE + offset + 1, 4);
			memcpy(&new_delete_recordnum, pagebuf + HEADER_AREA_SIZE + offset + 5, 4);
			//삭제된 공간이 저장 recordbuf보다 크기가 클 때 => 위치에 저장
			if (length >= strlen(recordbuf)) {
				memcpy(pagebuf + HEADER_AREA_SIZE + offset, recordbuf, strlen(recordbuf));
				writePage(fp, pagebuf, delete_pagenum);

				//삭제번호 갱신
				if (count == 1) { //header record 갱신
					fseek(fp, 8, SEEK_SET);
					fwrite(&new_delete_pagenum, 4, 1, fp);
					fwrite(&new_delete_recordnum, 4, 1, fp);
				}
				else { //레코드 삭제번호 갱신
					readPage(fp, pagebuf, pre_delete_pagenum);
					memcpy(&offset, pagebuf + 4 + 8 * pre_delete_recordnum, 4);
					memcpy(&length, pagebuf + 4 + 8 * pre_delete_recordnum + 4, 4);
					memcpy(pagebuf + HEADER_AREA_SIZE + offset + 1, &new_delete_pagenum, 4);
					memcpy(pagebuf + HEADER_AREA_SIZE + offset + 5, &new_delete_recordnum, 4);
					writePage(fp, pagebuf, pre_delete_pagenum);
				}
				break;
			}
			// 삭제된 공간이 저장 recordbuf보다 크기가 작을 때 =>linkedlist 사용해서 다음 삭제된 공간으로 이동
			else {
				pre_delete_pagenum = delete_pagenum;
				pre_delete_recordnum = delete_recordnum;
				delete_pagenum = new_delete_pagenum;
				delete_recordnum = new_delete_recordnum;
				continue;
			}
		}
	}
	free(pagebuf);
	free(recordbuf);
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE* fp, const char* id)
{
	char* recordbuf = (char*)malloc(MAX_RECORD_SIZE);
	char* pagebuf = (char*)malloc(PAGE_SIZE);
	char* idbuf = (char*)malloc(14);
	memset(idbuf, 0, 14);
	int pagenum; //전체 페이지수
	int recordnum; //전체 레코드수
	int delete_pagenum; // 가장최근 삭제레코드 페이지 번호
	int delete_recordnum; // 가장 최근 삭제레코드 레코드 번호
	int offset;
	int length;
	int page_record_num;
	int new_delete_pagenum;
	int	new_delete_recordnum;
	char ch;
	char delete_mark = '*';
	int ans = 0;
	fseek(fp, 0, SEEK_SET);
	fread(&pagenum, 4, 1, fp);
	fread(&recordnum, 4, 1, fp);
	fread(&delete_pagenum, 4, 1, fp);
	fread(&delete_recordnum, 4, 1, fp);
	for (int i = 0; i < pagenum; i++) {
		readPage(fp, pagebuf, i);
		memcpy(&page_record_num, pagebuf, 4);
		for (int j = 1; j <= page_record_num; j++) {
			memcpy(&offset, pagebuf + 4 + (j - 1) * 8, 4);
			memcpy(&length, pagebuf + 4 + (j - 1) * 8 + 4, 4);
			for(int k=0; k<14; k++) {
				memcpy(&ch, pagebuf + HEADER_AREA_SIZE + offset + k, 1);
				if (ch == '#') {
					break;
				}
				else {
					idbuf[k] = ch;
				}
			}
			if (strcmp(idbuf, id) == 0) {
				//삭제 레코드 관리
				memcpy(pagebuf + HEADER_AREA_SIZE + offset, &delete_mark, 1);
				memcpy(pagebuf + HEADER_AREA_SIZE + offset + 1, &delete_pagenum, 4);
				memcpy(pagebuf + HEADER_AREA_SIZE + offset + 5, &delete_recordnum, 4);
				writePage(fp, pagebuf, i);
				//헤더레코드 수정
				fseek(fp, 8, SEEK_SET);
				new_delete_pagenum = i;
				new_delete_recordnum = j - 1;
				fwrite(&new_delete_pagenum, 4, 1, fp);
				fwrite(&new_delete_recordnum, 4, 1, fp);
				ans++;
				break;
			}
			else
				continue;
		}
		if (ans == 1) {
			break;
		}
	}
	free(idbuf);
	free(pagebuf);
	free(recordbuf);
}

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
	FILE* fp;  // 레코드 파일의 파일 포인터
	if (exists(argv[2]) == 0) {
		fp = fopen(argv[2], "w+b");
		if (fp == NULL) {
			printf("file open error");
			return 1;
		}
		int pagenum=0; //전체 페이지수
		int recordnum=0; //전체 레코드수
		int delete_pagenum=-1; // 가장최근 삭제레코드 페이지 번호
		int delete_recordnum=-1; // 가장 최근 삭제레코드 레코드 번호
		fwrite(&pagenum, 4, 1, fp);
		fwrite(&recordnum, 4, 1, fp);
		fwrite(&delete_pagenum, 4, 1, fp);
		fwrite(&delete_recordnum, 4, 1, fp);
		char buf[PAGE_SIZE];
		memset(buf, 0, PAGE_SIZE);
		fwrite(buf, PAGE_SIZE, 1, fp);
	}
	else {
		fp = fopen(argv[2], "r+b");
		if (fp == NULL) {
			printf("file open error");
			return 1;
		}
	}

	if (strcmp(argv[1], "a")==0) {
		Person* p = (Person*)malloc(sizeof(Person));
		strcpy(p->id, argv[3]);
		strcpy(p->name, argv[4]);
		strcpy(p->age, argv[5]);
		strcpy(p->addr, argv[6]);
		strcpy(p->phone, argv[7]);
		strcpy(p->email, argv[8]);
		add(fp, p);
		free(p);
	}
	else if (strcmp(argv[1], "d")==0) {
		delete(fp, argv[3]);
	}

	fclose(fp);
	return 0;
}
