/*執行方法：
./myShell
>> ls -R /
ctr-c
>> ls /home/shiwulo -R
*/
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
/// color///
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"
 
/*
全域變數，放解析過後的使用者指令（字串陣列）
*/
char *argVect[256];
int child_pid = -1, wstatus,HZ;        // wstatus紀錄child的結束狀態
struct timespec startTime, endTime;
char *exeName;
sigjmp_buf jumpBuf;
struct rusage resUsage;
long int elapse1,elapse2;
const long int nspersec = 1000000000;

/*
parseString：將使用者傳進的命令轉換成字串陣列
str：使用者傳進的命令
cmd：回傳執行檔
*/

long int timespec2sec(struct timespec ts) {
    long int ns = ts.tv_nsec;
    return ns;
}

void parseString(char *str, char **cmd) {
	int idx = 0;
	char *retPtr;
	// printf("%s\n", str);
	retPtr = strtok(str, " \n");
	while (retPtr != NULL) {
	// printf("token =%s\n", retPtr);
	// if(strlen(retPtr)==0) continue;
	argVect[idx++] = retPtr;
	if (idx == 1)
	*cmd = retPtr;
	retPtr = strtok(NULL, " \n");
	}
	argVect[idx] = NULL;
}
 
//印出提示訊息
void printPrompt() {
	char cwd[256], hostName[256];
	char *showPath, *loginName;
	int homeLen = 0, pos = 0;

	//抓取主機名稱、用戶名稱
	loginName = getlogin();
	gethostname(hostName, 256);
	//底下程式碼製造要顯示的路徑字串，會參考"home"將"home"路徑取代為~
	getcwd(cwd, 256);
	pos = strspn(getenv("HOME"), cwd);
	homeLen = strlen(getenv("HOME"));
	// printf("cwd=%s, home=%s, pos=%d, prompt=%s", cwd, getenv("HOME"), pos,
	// &cwd[pos]);
	if (pos >= homeLen) {
	cwd[pos - 1] = '~';
	showPath = &cwd[pos - 1];
	} else
	showPath = cwd;
	//底下程式碼負責印出提示符號
	printf(LIGHT_GREEN "%s@%s:" BLU_BOLD "%s>> " NONE, loginName, hostName,
	 showPath);
	//因為沒有換行符號，因此必須fflush，否則linebuffer情況下，不會立即印出東西
	fflush(stdout); 
}

void handler(int sigNumber,siginfo_t *info,void *uncontext) {
	switch (sigNumber) {
		case SIGINT:
			if (child_pid > 0) {	
				int ret = kill(child_pid, sigNumber);
				if (ret == -1) {
				    	// system("touch kill_fault");   //for debugging
				    	perror("kill child");
				    	exit(1);
			 	}
				child_pid = -1;	
			} else {
				printf("\n");
				siglongjmp(jumpBuf, 1);
			}
			break;
		case SIGCHLD:
			clock_gettime(CLOCK_REALTIME, &endTime);
			printf(RED "real: " YELLOW "%ld.%ldsec\n",
			       (endTime.tv_sec - startTime.tv_sec)/ 10000000000 +
				   (endTime.tv_nsec - startTime.tv_nsec) / 1000000000,
			       ((endTime.tv_sec - startTime.tv_sec) +
				(endTime.tv_nsec - startTime.tv_nsec)) %
				   1000000);
			HZ = sysconf(_SC_CLK_TCK); //取得utime, stime的單位
			printf(RED "user: " YELLOW "%04ld.%ld sec\n", resUsage.ru_utime.tv_sec / HZ,
			       resUsage.ru_utime.tv_sec  % HZ);
			printf(RED "sys : " YELLOW "%04ld.%ld sec\n", resUsage.ru_stime.tv_sec / HZ,
			       resUsage.ru_stime.tv_sec % HZ);
			//印出結束狀態
			if (WIFEXITED(wstatus)) { // child執行結束
			  printf(RED "return value of " YELLOW "%s" RED " is " YELLOW "%d\n",
				 exeName, WEXITSTATUS(wstatus));
			} else if (WIFSIGNALED(wstatus)) { // child被signal打斷
			  printf(RED "the child process was terminated by a signal " YELLOW
				     "%d" RED ", named " YELLOW "%s.\n",
				 WTERMSIG(wstatus),
				 sys_siglist[WTERMSIG(wstatus)]);
			}
			printf(NONE);
			child_pid = -1;

			break;
		default: //印出不感興趣的signal名稱
		        printf(RED "signal # is %d\n",sigNumber);
			siglongjmp(jumpBuf, 1);
	}
}
 
int main(int argc, char **argv) {

	char cmdLine[4096];

	struct timespec startTime, endTime; //計算child的執行時
	printf(YELLOW "pid is %d\n", getpid()); //幫助除錯用

	struct sigaction sa;
	sa.sa_sigaction=handler;
	sigaction(SIGINT, &sa , NULL);
	sigaction(SIGCHLD, &sa , NULL);
	sigaction(SIGQUIT, &sa , NULL);
	sigaction(SIGTSTP, &sa , NULL);
	 
  	/*無窮迴圈直到使用者輸入exit */
	while (1) {

		sigsetjmp(jumpBuf, 1);
	    	if (child_pid == -1) printPrompt();

		fgets(cmdLine, 4096, stdin);
		//printf("cmdLine = %s\n",cmdLine);
		if (strlen(cmdLine)>1)  //判斷長度是否大於1，判斷「使用者無聊按下enter鍵」
		    parseString(cmdLine, &exeName);
		else
		    continue;
		if (strcmp(exeName, "exit") == 0)   //內建指令exit
		    break;
		if (strcmp(exeName, "^c") == 0) {   //使用者按下control-c，^c是由signal handler放入
			printf("\n");
			continue;
		}
		if (strcmp(exeName, "cd") == 0) {   //內建指令cd
		    if (strcmp(argVect[1], "~")==0)
		        chdir(getenv("HOME"));
		    else
		        chdir(argVect[1]);
		    continue;
		}
	   	child_pid = fork(); //除了exit, cd，其餘為外部指令
	    	if (child_pid == 0) {
			clock_gettime(CLOCK_REALTIME,&startTime);			
			if (execvp(exeName, argVect) == -1) {
        			perror("myShell");
        			exit(errno * -1);
      			}
		}else{
			//wait(&wstatus);
			wait3(&wstatus, 0, &resUsage);

		}		   
  	}	
	return 0;
}
