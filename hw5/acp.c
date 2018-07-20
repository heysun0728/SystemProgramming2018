#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#define BUF_SIZE 4096
int main(int argc,char*argv[]){
	char buffer[BUF_SIZE];
	FILE *fp,*finput,*foutput;

	//open file1,file2
	finput=fopen(argv[1],"r");
	if(finput==NULL) perror("open file error:");
	foutput=fopen(argv[2],"w+");
	if(foutput==NULL) perror("open file2 error:");

	//make temp file
	char tmpStr[] = "/tmp/acp_YingRan_XXXXXX";
	mkstemp(tmpStr);
	fp=fopen(tmpStr,"w+");
	if(fp==NULL) perror("make tmpfile error:");
		
	//copy file1 content to temp
	int ret;
	while(1){
		ret=fread(buffer,1,BUF_SIZE,finput);
		ret=fwrite(buffer,1,ret,fp);
		if(feof(finput)) break;
	}
	fflush(fp);	
	fsync(fileno(fp));

	//pause	
	printf("System is pause now. Please enter any character to continue.\n");
	char c=getchar();
	
	//copy temp content to file2
	fseek(fp,0,SEEK_SET);
	while(1){
		ret=fread(buffer,1,BUF_SIZE,fp);
		ret=fwrite(buffer,1,ret,foutput);
		if(feof(fp)) break;
	}

	//remove temp file and close other file
	remove(tmpStr);
	fclose(finput);fclose(fp);fclose(foutput);
	return 0;
}
