#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <limits.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
// #include <sys/sem.h>


#define VALID 1
#define INVALID 0
#define BASEID 1000
#define MMUDESTN 10
#define SCHEDDESTN 11
#define MASTERDESTN 12 

#define PAGE_FAULT_HANDLED 1
#define TERMINATED 2

#define PAGE_FAULT_OCCURED -1
#define INVALID_PAGE_REFERENCE -2
#define PROCESSCOMPLETED -9

#define SHMKEY "master.c"



struct ptablerec{
    int frame_no;
    int state;
    int count;
};

struct pInfo{
    int m;
    int max_alloc;
    int curr_alloc;
};

struct msgbuff{
    long mtype;
    int id;
    int page_no;
    int frame_no;
};


int min(int a, int b){
    return a<b?a:b;
}

int max(int a, int b){
    return a>b?a:b;
}   
