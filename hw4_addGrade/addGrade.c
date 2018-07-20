#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#define BUF_SIZE 4096
int main(){
	char buffer[BUF_SIZE];
	int fd,ret;
	int id,grade[5];
	ssize_t numOut;
	fd=open("grade.txt",O_WRONLY|O_APPEND|O_CREAT,S_IRUSR| S_IWUSR);	
	while(1){
		scanf("%d%d%d%d%d%d",&id,&grade[0],&grade[1],&grade[2],&grade[3],&grade[4]);
		sprintf(buffer,"%d\t%d\t%d\t%d\t%d\t%d\n",id,grade[0],grade[1],grade[2],grade[3],grade[4]);
		if(fd==-1){
			perror("cannot open the file for write"); 
			exit(1);
		}
		numOut=write(fd,buffer,strlen(buffer));
		if(flock(fd,LOCK_EX|LOCK_NB)!=0) printf("Critical Section\n");
		sleep(5);
		printf("done\n");
		ret=flock(fd,LOCK_UN);
	}		
	close(fd);
	return 0;
}
