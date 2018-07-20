#include "f.h"
char* f[1001];
int z[MAX_SIZE];
void sum(int index,char *x,char *y){ //f[index]=x+y
	int i,j,k,sum;
	i=strlen(x)-1;
	j=strlen(y)-1;
	k=0;
	//sum
	while(1){
		if(i<0&&j<0)break;
		sum=z[k];
		if(i>=0) sum+=x[i]-'0';
		if(j>=0) sum+=y[j]-'0';
		z[k]=sum%10;
		z[k+1]+=sum/10;
		i--;j--;k++;
	}
	//put answer into f and print
	f[index]=malloc((k+5)*sizeof(char));
	i=MAX_SIZE-1;
	j=0;
	while(z[i]==0) i--; 	
	for(;i>=0;i--){
		f[index][j++]=z[i]+'0';//put ans in f[index] 
		printf("%d",z[i]);//print ans
		z[i]=0;//set z to zero
	}
	printf("\n");
}
void fib(int i){ //call and print fib[i]
	if(i<=1){
		if(i==0) f[i]="0";
		if(i==1) f[i]="1";
		printf("%s\n",f[i]);
	}
	else sum(i,f[i-1],f[i-2]);
}
int main(){

	for(int i=0;i<1000;i++){
		printf("(%d):\n",i);
		fib(i);
	}
	
	return 0;
}

