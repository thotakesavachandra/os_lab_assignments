#include "includes.h"

// int m,k,f;
// struct pInfo* pinfo;

int ready_id, sched_mmu_id;
struct msgbuff sendbuff, recvbuff;


void send_message_proc(int id, int page_no, int frame_no){
    sendbuff.mtype = id+BASEID;
    sendbuff.id = id;
    sendbuff.page_no = page_no;
    sendbuff.frame_no = frame_no;
    int rst = msgsnd(ready_id, &sendbuff, sizeof(struct msgbuff), 0);
    if(rst==-1){
        perror("msgsnd");
        exit(0);
    }
}

void add_to_queue(int id){
    sendbuff.mtype = SCHEDDESTN;
    sendbuff.id = id;
    sendbuff.page_no = -1;
    sendbuff.frame_no = -1;
    int rst = msgsnd(ready_id, &sendbuff, sizeof(struct msgbuff), 0);
    if(rst==-1){
        perror("msgsnd");
        exit(0);
    }
}

void notify_master(){
    sendbuff.mtype = MASTERDESTN;
    sendbuff.id = -1;
    sendbuff.page_no = -1;
    sendbuff.frame_no = PROCESSCOMPLETED;
    int rst = msgsnd(ready_id, &sendbuff, sizeof(struct msgbuff), 0);
    if(rst==-1){
        perror("msgsnd");
        exit(0);
    }
}


int main(char argc, char* argv[]){
    
    int k = atoi(argv[1]);
    ready_id = atoi(argv[2]);
    sched_mmu_id = atoi(argv[3]);

    printf("Scheduler Started\n");
    int rem = k;

    while(rem>0){
        int rst = msgrcv(ready_id, &recvbuff, sizeof(struct msgbuff), SCHEDDESTN, 0);
        printf("Scheduler active\n");
        int id = recvbuff.id;
        // signal the first process to start
        send_message_proc(id, -1, -1);

        // wait for a message from mmu about the status of the process
        rst = msgrcv(sched_mmu_id, &recvbuff, sizeof(struct msgbuff), SCHEDDESTN, 0);

        int status = recvbuff.frame_no;

        if(status==PAGE_FAULT_HANDLED){
            add_to_queue(id);   
        }
        else if(status==TERMINATED){
            // do nothing 
            rem--;
            printf("Scheduler: Process %d terminated rem:%d\n",id,rem);
        }
    }
    notify_master();
    return 0;

}