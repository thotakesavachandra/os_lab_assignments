/************** OPERATING SYSTEMS ASSIGNMENT 4 *************************

 * Name          - Thota Kesava Chandra
 * Roll Number   - 21CS30056

 **********************************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>	
#include <stdlib.h>
#include <time.h>


void child(int id,int *m){
    char indent[] = "\t\t\t\t\t";
    printf("%sConsumer %d is Alive\n",indent,id);
    int checkSum = 0;
    int count = 0;
    while(1){
        while(m[0]==0);
        if(m[0]==-1) break;
        if(m[0]==id){
            count++;
            int val = m[1];
            #ifdef VERBOSE
                printf("%sConsumer %d reads %d\n",indent,id,val);
            #endif
            checkSum += val;
            m[0] = 0;
        }
    }
    printf("%sConsumer %d has read %d items: Checksum = %d\n",indent,id,count,checkSum);
    shmdt(m);
}

int newNum(int choice){
    if(choice==0){  // 0 for random 3 digit value
        int num = rand()%900;
        return num+100;
    }
    return rand()%choice+1; // any other value for random value between 1 and choice
}


void parent(int n,int t,int *m){
    int * checkSum = (int*) malloc(sizeof(int)*(n+1));
    int * count = (int*) malloc(sizeof(int)*(n+1));
    for(int i=1;i<=n;i++){
        checkSum[i] = 0;
        count[i] = 0;
    }

    printf("Producer is Alive\n");
    for(int i=0;i<t;i++){
        while(m[0]!=0);
        int c = newNum(n);
        int val = newNum(0);
        checkSum[c] += val;
        count[c]++;
        #ifdef VERBOSE
            printf("Producer produces %d for Consumer %d\n",val,c);
        #endif

        m[0] = c;

        #ifdef SLEEP
            usleep(10);
        #endif
        
        m[1] = val;
    }
    while(m[0]!=0);
    m[0] = -1;

    for(int i=0;i<n;i++){
        wait(NULL);
    }
    printf("Producer has produced %d items\n",t);
    for(int i=1;i<=n;i++){
        printf("%d items for Consumer %d: Checksum = %d\n",count[i],i,checkSum[i]);
    }

    shmdt(m);
}


int main()
{
    srand(time(0));
	int shmid, status;
	int *a, *b;
	int i;

	shmid = shmget(IPC_PRIVATE, 2*sizeof(int), 0777|IPC_CREAT);
    int* m = (int*) shmat(shmid,0,0);
    m[0] = 0;
    
    
    int n,t;
    printf("Enter the number of consumers: ");
    scanf("%d", &n);
    printf("Enter the number of items: ");
    scanf("%d", &t);
    for(int i=1;i<=n;i++){
        if(fork()==0){
            child(i,m);
            exit(0);
        }
    }
    parent(n,t,m);
    shmctl(shmid, IPC_RMID, 0);
    return 0;
}

