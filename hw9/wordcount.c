#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc,char* argv[]){
	int pipefd[2],i,ret1,ret2,status;
	char buf[4096];
	pipe(pipefd);
	ret1=fork();	
	if(ret1==0){
		close(0);
		dup(pipefd[0]);
		close(pipefd[0]);
		close(pipefd[1]);
		char* param[]={"wc",NULL};
		if (execv("/usr/bin/wc",param) == -1) {
			perror("myShell");
			exit(errno * -1);
      		}
	}
	close(pipefd[0]);
	ret2=fork();
	if(ret2==0){
		close(1);
		dup(pipefd[1]);
		close(pipefd[1]);
		close(pipefd[0]);
		char* param[]={"cat",argv[1],NULL};
		if (execv("/bin/cat",param) == -1) {
			perror("myShell");
			exit(errno * -1);
		}	
	}
	close(pipefd[1]);
	waitpid(ret1, &status, 0);	
	return 0;
}
