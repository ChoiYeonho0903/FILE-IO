#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
		FILE *fp = fopen(argv[1], "r");
		FILE *nfp = fopen(argv[2], "w");
		char buffer[10]={0,};
		int count =0;

		while(feof(fp) == 0){
			count = fread(buffer, sizeof(char), 10, fp);
			fwrite(buffer, sizeof(char), count, nfp);
			memset(buffer, 0, 10);
		}
		fclose(fp);
		fclose(nfp);
		return 0;

}
