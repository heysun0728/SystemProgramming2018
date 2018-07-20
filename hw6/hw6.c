#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/inotify.h>
#include <unistd.h>
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int level = 0;
char wd[1000][10000];
typedef struct{
	int IsDir;
	char name[35];
}file;
void printfile(file* f,int num,char* pathName);
void listDir(char* pathName);
void printName(char* name,int IsDir) {
    for (int i=1; i < level; i++)
        printf("  ");
    printf("|%s\n", name);
}

void printfile(file* f,int num,char* pathName){
    char* newPathName = (char*)malloc(PATH_MAX);
    int i;
    for(i=0;i<num;i++){
    	printName(f[i].name,f[i].IsDir);
    	if (f[i].IsDir == 1) {
            sprintf(newPathName, "%s/%s", pathName, f[i].name);
       	    listDir(newPathName);
    	}
    }
}

int cmp(const void *arg1, const void *arg2) {
  return  strcmp((*(file *)arg1).name,(*(file *)arg2).name)>0;
}

void listDir(char* pathName)
{
    file f[500];
    level++;
    int num=0;
    DIR* curDir = opendir(pathName);
    assert(curDir!=NULL);
    struct dirent entry;
    struct dirent* result;
    int ret;
    ret = readdir_r(curDir, &entry, &result);
    assert(ret == 0); 
    while(result != NULL) {
	strcpy(f[num].name,entry.d_name);
        if (entry.d_type == DT_DIR) {
	    if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0) {
		f[num].IsDir=0;
	    }else{
		f[num].IsDir=1;
	    }
        }else{
	    f[num].IsDir=0;
	}
        ret = readdir_r(curDir, &entry, &result);
        assert(ret == 0);  
	num++;   
    }
    qsort(f,num, sizeof(file), cmp);
    printfile(f,num,pathName);
    closedir(curDir);
    level--;
}

void printInotifyEvent(struct inotify_event* event) {
    char buf[4096]="";
    sprintf(buf, "[%s] ", wd[event->wd]);
    strncat(buf, "{", 4096);
    if (event->mask & IN_ACCESS)         strncat(buf, "ACCESS, ", 4096);
    if (event->mask & IN_ATTRIB)         strncat(buf,"ATTRIB, ", 4096);
    if (event->mask & IN_CLOSE_WRITE)    strncat(buf,"CLOSE_WRITE, ", 4096);
    if (event->mask & IN_CLOSE_NOWRITE)  strncat(buf,"CLOSE_NOWRITE, ", 4096);
    if (event->mask & IN_CREATE)         strncat(buf,"CREATE, ", 4096);
    if (event->mask & IN_DELETE)         strncat(buf,"DELETE, ", 4096);
    if (event->mask & IN_DELETE_SELF)    strncat(buf,"DELETE_SELF, ", 4096);
    if (event->mask & IN_MODIFY)         strncat(buf,"MODIFY, ", 4096);
    if (event->mask & IN_MOVE_SELF)      strncat(buf,"MOVE_SELF, ", 4096);
    if (event->mask & IN_MOVED_FROM) strncat(buf,"MOVED_FROM, ", 4096);
    if (event->mask & IN_MOVED_TO)       strncat(buf,"MOVED_TO, ", 4096);
    if (event->mask & IN_OPEN)           strncat(buf,"OPEN, ", 4096);
    if (event->mask & IN_IGNORED)        strncat(buf,"IGNORED, ", 4096);
    if (event->mask & IN_ISDIR)          strncat(buf,"ISDIR, ", 4096);
    if (event->mask & IN_Q_OVERFLOW) strncat(buf,"Q_OVERFLOW, ", 4096);
    buf[strlen(buf)-2]='\0';
    strncat(buf, "}", 4096);
    sprintf(buf, "%s cookie=%d", buf, event->cookie);
    if (event->len>0)
        sprintf(buf, "%s name = %s\n", buf, event->name);
    else
        sprintf(buf, "%s name = null\n", buf);
    printf("%s", buf);
}

int main(int argc, char** argv) {
    listDir(argv[1]);
    int fd, num, ret, i;
    char* p;
    char inotify_entity[BUF_LEN];
    fd = inotify_init();
    ret=inotify_add_watch(fd, argv[1], IN_ALL_EVENTS);
    strcpy(wd[ret], argv[1]);
    while(1) {
        num = read(fd, inotify_entity, BUF_LEN);
        for (p = inotify_entity; p < inotify_entity + num; ) {
            printInotifyEvent((struct inotify_event *) p);
            p+=sizeof(struct inotify_event) + ((struct inotify_event *)p)->len;
        }
    }
}
