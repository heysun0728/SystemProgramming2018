#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <sys/mman.h>
static int *sum;
int main(){
	sum=mmap(NULL, sizeof *sum, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);	
	int status=0,n,i,n1;
	pid_t pid[4];	
	pid[0]=fork();
	pid[1]=fork();
	pid[2]=fork();
	pid[3]=fork();
	
	//how many child i need to wait
	n=4;
	for(i=0;i<4;i++){
		if(pid[i]==0) n--;
	}

	//count num
	if(n1==4) (*sum)=0;
	(*sum)++;
	
	//wait
	n1=n;
	while(n>0){		
		int ret = wait(&status);
		--n;
	}

	if(n1==4) printf("%d\n",*sum);

	return 0;
}
