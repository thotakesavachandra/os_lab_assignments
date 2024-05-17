#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>	
#include <sys/sem.h>



int checkOrder(int* a,int * order,int n){
    int pos[n];
    for(int i=0;i<n;i++){
        pos[order[i]] = i;
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(a[i*n+j]==1){
                if(pos[i]>=pos[j]){
                    printf("Error: %d %d\n",i,j);
                    return 0;
                }
            }
        }
    }
    return 1;
}


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


void boss(){
    FILE* fp;
    fp = fopen("graph.txt","r");
    int n;
    fscanf(fp,"%d",&n);


    int keys[6];
    getKeys(keys,6);

    int aId = shmget(keys[0], n*n*sizeof(int), 0777|IPC_CREAT);
    int *a = (int*) shmat(aId, 0, 0);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            fscanf(fp,"%d",&a[i*n+j]);
        }
    }
    fclose(fp);

    int tId = shmget(keys[1], n*sizeof(int), 0777|IPC_CREAT);
    int *t = (int*) shmat(tId, 0, 0);

    int idxId = shmget(keys[2], sizeof(int), 0777|IPC_CREAT);
    int *idx = (int*) shmat(idxId, 0, 0);
    idx = 0;

    int mtxId = semget(keys[3], 1, 0777|IPC_CREAT);
    semctl(mtxId, 0, SETVAL, 1);
    
    int syncId = semget(keys[4],n*sizeof(int),0777|IPC_CREAT);

    for(int i=0;i<n;i++){
        semctl(syncId, i, SETVAL, 0);
    }

    int ntfyId = semget(keys[5], n, 0777|IPC_CREAT);
    for(int i=0;i<n;i++){
        semctl(ntfyId, i, SETVAL, 0);
    }

    printf("+++ Boss: Setup done...\n");


    // waiting for all workers to complete
    for(int i=0;i<n;i++){
        wait_sem(ntfyId,i,1);  
    }
    
    int flag = checkOrder(a,t,n);
    // Checking if the topological order is correct

    if(flag==1){
        printf("+++ Topological sorting of the vertices\n");
        for(int i=0;i<n;i++){
            printf("%d ",t[i]);
        }
        printf("\n");
        printf("+++ Boss: Well done, my team...\n");
    }
    else{
        printf("Failed\n");
    }


    
    shmdt(a);
    shmctl(aId, IPC_RMID, 0);
    shmdt(t);
    shmctl(tId, IPC_RMID, 0);
    shmdt(idx);
    shmctl(idxId, IPC_RMID, 0);

    semctl(mtxId, 0, IPC_RMID, 0);

    semctl(syncId, 0, IPC_RMID, 0);

    semctl(ntfyId, 0, IPC_RMID, 0);
}

int main(int argc, char* argv[])
{
    boss();
}
