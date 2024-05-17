#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>	/* Include this to use shared memory */
#include <sys/sem.h>	/* Include this to use semaphores */


void getKeys(int keys[],int n){
    for(int i=0;i<n;i++){
        keys[i] = ftok("boss", i);
    }
}


void wait_sem(int semId,int idx,int n){
    struct sembuf pop;
    pop.sem_num = idx;
    pop.sem_flg = 0;
    pop.sem_op = -n ;
    semop(semId, &pop, 1);
}

void signal_sem(int semId,int idx,int n){
    struct sembuf vop;
    vop.sem_num = idx;
    vop.sem_flg = 0;
    vop.sem_op = n ;
    semop(semId, &vop, 1);
}


void worker(int n,int id){

    int keys[6];
    getKeys(keys,6);

    int aId = shmget(keys[0], 0, 0);
    int *a = (int*) shmat(aId, 0, 0);
    int tId = shmget(keys[1], 0, 0);
    int *t = (int*) shmat(tId, 0, 0);
    int idxId = shmget(keys[2], 0, 0);
    int *idx = (int*) shmat(idxId, 0, 0);

    int mtxId = semget(keys[3], 0, 0);
    int syncId = semget(keys[4], 0, 0);
    int ntfyId = semget(keys[5], 0, 0);

    // Enter the critical section
    {
        int nParents = 0;
        for(int i=0;i<n;i++){
            if(a[i*n+id]==1){
                nParents++;
            }
        }
        
        wait_sem(syncId,id,nParents);

    }
    
    // Critical section
    {

        wait_sem(mtxId,0,1);

        t[*idx] = id;
        *idx = *idx + 1;
    
        // relaxing the children
        for(int j=0;j<n;j++){
            if(a[id*n+j]==1){
                signal_sem(syncId,j,1);
            }
        }

        // exiting the critical section
        signal_sem(mtxId,0,1);
    }

    // notifying the boss
    signal_sem(ntfyId,id,1);


    shmdt(a);
    shmdt(t);
    shmdt(idx);
}



int main(int argc, char* argv[])
{
    if(argc<3){
        printf("Usage: %s <n> <id>\n",argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int id = atoi(argv[2]);
    worker(n,id);
}
