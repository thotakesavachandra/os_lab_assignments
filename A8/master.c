#include "includes.h"
#define PINFOKEY 1
#define PTABLERECKEY 2
#define FFLKEY 3

#define READYQUEUEKEY 4
#define SCHEDMMUKEY 5
#define PROCMMUKEY 6

int m,k,f;
struct pInfo* pinfo;
struct ptablerec* ptable;
int * ffl;
int pInfoId, pTableId, fflId;
int ready_id, sched_mmu_id, proc_mmu_id;

struct msgbuff sendbuff, recvbuff;

void initSharedMemory(){
    
    pInfoId = shmget( ftok(SHMKEY,PINFOKEY), sizeof(struct pInfo)*m, IPC_CREAT | 0666);
    pinfo = (struct pInfo*)shmat(pInfoId, NULL, 0);
    
    pTableId = shmget( ftok(SHMKEY,PTABLERECKEY), sizeof(struct ptablerec)*m*k, IPC_CREAT | 0666);
    ptable = (struct ptablerec*)shmat(pTableId, NULL, 0);

    fflId = shmget( ftok(SHMKEY, FFLKEY), sizeof(int)*(f+1), IPC_CREAT | 0666);
    ffl = (int*) shmat(fflId, NULL, 0);
    
    for(int id=0; id<k; id++){
        for(int j=0; j<m; j++){
            ptable[id*m+j].frame_no = -1;
            ptable[id*m+j].state = INVALID;
            ptable[id*m+j].count = -1;
        }
    }

    ffl[0] = f;
    for(int i=1;i<=f;i++){
        ffl[i] = i-1;
    }

    // -- message queues
    ready_id = msgget(ftok(SHMKEY,READYQUEUEKEY), IPC_CREAT | 0666);
    sched_mmu_id = msgget(ftok(SHMKEY,SCHEDMMUKEY), IPC_CREAT | 0666);
    proc_mmu_id = msgget(ftok(SHMKEY,PROCMMUKEY), IPC_CREAT | 0666);

}

int createScheduler(){
    int spid = fork();
    if(spid==0){
        // pass all the arguments and execlp the scheduler
        /*
            int k = atoi(argv[1]);
            ready_id = atoi(argv[2]);
            sched_mmu_id = atoi(argv[3]);
        */
        char buf1[20],buf2[20],buf3[20];
        sprintf(buf1,"%d",k);
        sprintf(buf2,"%d",ready_id);
        sprintf(buf3,"%d",sched_mmu_id);
        execlp("./sched","./sched",buf1,buf2,buf3,NULL);

        exit(0);
    }
    return spid;
}

int createMMU(){
    int mpid = fork();
    if(mpid==0){
        // pass all the arguments and execlp the mmu
        /*
            m = atoi(argv[1]);
            k = atoi(argv[2]);
            f = atoi(argv[3]);
            int pInfoId = atoi(argv[4]);
            int pTableId = atoi(argv[5]);
            int fflId = atoi(argv[6]);
            sched_mmu_id = atoi(argv[7]);
            proc_mmu_id = atoi(argv[8]);
  
        */
        char buf1[20],buf2[20],buf3[20],buf4[20],buf5[20],buf6[20],buf7[20],buf8[20];
        sprintf(buf1,"%d",m);
        sprintf(buf2,"%d",k);
        sprintf(buf3,"%d",f);
        sprintf(buf4,"%d",pInfoId);
        sprintf(buf5,"%d",pTableId);
        sprintf(buf6,"%d",fflId);
        sprintf(buf7,"%d",sched_mmu_id);
        sprintf(buf8,"%d",proc_mmu_id);
        // execlp("./mmu","./mmu",buf1,buf2,buf3,buf4,buf5,buf6,buf7,buf8,NULL);
        // exec in xterm
        execlp("xterm","xterm","-hold","-e","./mmu",buf1,buf2,buf3,buf4,buf5,buf6,buf7,buf8,NULL);

        exit(0);
    }
    return mpid;
}

void createProcesses(){
    
    // 
    int total_req = 0;
    // int total_alloc = 0;


    for(int id=0; id<k; id++){
        pinfo[id].m = rand()%m+1;
        total_req += pinfo[id].m;
        pinfo[id].curr_alloc = 0;
    }
    for(int id=0; id<k; id++){
        pinfo[id].max_alloc = max(1, (f*m)/total_req);
    }

    printf("\tM valuess-----\n");
    for(int id=0; id<k; id++){
        printf("\t%d\n",pinfo[id].m);
    }

    // now generate 
    for(int id=0; id<k; id++)
	{
		int rlen = rand()%(8*pinfo[id].m) + 2*pinfo[id].m + 1;
		char rstring[m*20*40];
		printf("rlen = %d\n",rlen);
		int l = 0;
		for(int j=0;j<rlen;j++)
		{
			int r;
			r = rand()%(pinfo[id].m);
			float p = (rand()%100)/100.0;
			if(p < 0.01)
			{
				r = rand()%(1000*m) + pinfo[id].m;
			}
			l += sprintf(rstring+l,"%d|",r);
		}
		printf("Ref string = %s\n",rstring);
		if(fork() == 0)
		{
            // create process by exec 
            /*
                id = atoi(argv[1]);
                ready_id = atoi(argv[2]);
                proc_mmu_id = atoi(argv[3]);
                conv_ref_pg_no(argv[4]);
            */

			char buf1[20],buf2[20],buf3[20],buff4[sizeof(rstring)];
            sprintf(buf1,"%d",id);
            sprintf(buf2,"%d",ready_id);
            sprintf(buf3,"%d",proc_mmu_id);
            sprintf(buff4,"%s",rstring);
            execlp("./proc","./proc",buf1,buf2,buf3,buff4,NULL);
            

			exit(0);

		}
		
		usleep(250*1000);	
	}
}

void freeResources(){
    shmdt(pinfo);
    shmdt(ptable);
    shmdt(ffl);
    shmctl( ftok(SHMKEY,PINFOKEY), IPC_RMID, NULL);
    shmctl( ftok(SHMKEY,PTABLERECKEY), IPC_RMID, NULL);
    shmctl( ftok(SHMKEY,FFLKEY), IPC_RMID, NULL);

    msgctl( ftok(SHMKEY,READYQUEUEKEY), IPC_RMID, NULL);
    msgctl( ftok(SHMKEY,SCHEDMMUKEY), IPC_RMID, NULL);
    msgctl( ftok(SHMKEY,PROCMMUKEY), IPC_RMID, NULL);
}

void signalHandler(int sig){
    sleep(1);
    printf("\n❌ Keyboard Interrupt\n");

    freeResources();
    exit(0);
}

int main(int argc, char* argv[]){
    // take input of k m f from user
    if(argc!=4){
        printf("Usage: ./master <k> <m> <f>\n");
        exit(0);
    }
    k = atoi(argv[1]);
    m = atoi(argv[2]);
    f = atoi(argv[3]);

    if(k<=0 || m<=0 || f<=0 || f<k){
        printf("Invalid input\n");
        exit(0);
    }

    // printf("\tReached here\n");
    
    initSharedMemory();
    signal(SIGINT, signalHandler);
    createScheduler();
    createMMU();
    createProcesses();
    
    printf("\tWaiting for message from scheduler\n");
    int rst = msgrcv(ready_id, &recvbuff, sizeof(struct msgbuff), MASTERDESTN, 0);
    {
        // sending message to mmu to terminate
        sendbuff.mtype = MMUDESTN;
        sendbuff.id = -1;
        sendbuff.page_no = -1;
        sendbuff.frame_no = -1;
        int rst = msgsnd(proc_mmu_id, &sendbuff, sizeof(struct msgbuff), 0);
    }
    printf("\tAll Processes terminated\n");

    freeResources();
    printf("\tResources freed and master exiting\n");
    return 0;
}