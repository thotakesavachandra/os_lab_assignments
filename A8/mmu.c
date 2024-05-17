#include "includes.h"

struct pInfo* pinfo;
struct ptablerec* ptable;
int* ffl;
int m,k,f;
int sched_mmu_id, proc_mmu_id;
int count = 0;
int * page_fault_count;

struct msgbuff recvbuff;
struct msgbuff sendbuff;

int fd;
char buff[100];


void send_message_proc(int id, int page_no, int frame_no){
    sendbuff.mtype = id+BASEID;
    sendbuff.id = id;
    sendbuff.page_no = page_no;
    sendbuff.frame_no = frame_no;
    int rst = msgsnd(proc_mmu_id, &sendbuff, sizeof(struct msgbuff), 0);
    if(rst==-1){
        printf("Error in sending message to proc\n");
        perror("msgsnd");
        fflush(stdout);
        exit(0);
    }
}

void send_message_sched(int message){
    sendbuff.mtype = SCHEDDESTN;
    sendbuff.id = -1;
    sendbuff.page_no = -1;
    sendbuff.frame_no = message;
    int rst = msgsnd(sched_mmu_id, &sendbuff, sizeof(struct msgbuff), 0);
    if(rst==-1){
        printf("Error in sending message to sched\n");
        perror("msgsnd");
        fflush(stdout);
        exit(0);
    }
}


void pageReplacement(int id, int page_no){
    int mini=INT_MAX;
    int mini_idx = -1;
    for(int j=0;j<pinfo[id].m; j++){
        if(ptable[id*m+j].state == VALID){
            if(ptable[id*m+j].count < mini){
                mini = ptable[id*m+j].count;
                mini_idx = j;
            }
        }
    }

    int victim = ptable[id*m+mini_idx].frame_no;

    ptable[id*m+mini_idx].state = INVALID;
    ptable[id*m+mini_idx].frame_no = -1;
    ptable[id*m+mini_idx].count = -1;

    ptable[id*m+page_no].state = VALID;
    ptable[id*m+page_no].frame_no = victim;
    ptable[id*m+page_no].count = count;

    return;
}

int handle_request(int id, int page_no){
    // handle the request
    
    if( ptable[id*m+page_no].state == VALID){
        // send message to proc about this
        printf("Page already in memory (%d, %d, %d)\n", id, page_no, ptable[id*m+page_no].frame_no);
        fflush(stdout);
        send_message_proc(id, page_no, ptable[id*m+page_no].frame_no);
        return 1;
    }
    printf("Handling page fault\n");
    sprintf(buff,"Page fault sequence - (%d,%d)\n", id, page_no);
    printf("%s",buff);
    write(fd, buff, strlen(buff));

    page_fault_count[id]++;
    send_message_proc(id, page_no, PAGE_FAULT_OCCURED);

    if(pinfo[id].curr_alloc < pinfo[id].max_alloc){
        // quota available
        // take some from free frame list
        int frame = ffl[ffl[0]];
        ffl[0]--;
        pinfo[id].curr_alloc++;
        ptable[id*m+page_no].state = VALID;
        ptable[id*m+page_no].frame_no = frame;
        ptable[id*m+page_no].count = count;
        printf("Allocating Free Frame (%d, %d, %d)\n", id, page_no, frame);
        fflush(stdout);

        // send_message_proc(id, page_no, ptable[id*m+page_no].frame_no);
        // return 1;
    }
    else{
        pageReplacement(id, page_no);
    }
    

    // send message to scheduler
    send_message_sched(PAGE_FAULT_HANDLED);
    

    
}


void take_away_frames(int id){
    for(int j=0;j<pinfo[id].m; j++){
        if(ptable[id*m+j].state == VALID){
            ffl[0]++;
            ffl[ffl[0]] = ptable[id*m+j].frame_no;
            ptable[id*m+j].state = INVALID;
            ptable[id*m+j].frame_no = -1;
            ptable[id*m+j].count = -1;
        }
    }
}

void freeResources(){
    close(fd);
    free(page_fault_count);
    shmdt(pinfo);
    shmdt(ptable);
    shmdt(ffl);
}

void signal_handler(int sig){
    freeResources();
    exit(1);
}


int main(int argc, char* argv[]){

    if(argc!=9){
        printf("Usage: ./mmu <m> <k> <f> <pInfoId> <pTableId> <fflId> <sched_mmu_id> <proc_mmu_id>\n");
        exit(0);
    }
    printf("MMU started\n");

    fd = open("result.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    

    
    {
        m = atoi(argv[1]);
        k = atoi(argv[2]);
        f = atoi(argv[3]);
        int pInfoId = atoi(argv[4]);
        int pTableId = atoi(argv[5]);
        int fflId = atoi(argv[6]);
        sched_mmu_id = atoi(argv[7]);
        proc_mmu_id = atoi(argv[8]);

        pinfo = (struct pInfo*)shmat(pInfoId, NULL, 0);
        ptable = (struct ptablerec*)shmat(pTableId, NULL, 0);  
        ffl = (int*)shmat(fflId, NULL, 0);      

        page_fault_count = (int*)malloc(sizeof(int)*k);
        for(int i=0;i<k;i++){
            page_fault_count[i] = 0;
        }

    }

    signal(SIGINT, signal_handler);

    // recv message from proc


    while(1){
        int rst = msgrcv(proc_mmu_id, &recvbuff, sizeof(struct msgbuff), MMUDESTN, 0);
        if(rst==-1){
            perror("msgrcv");
            exit(0);
        }
        
        if(recvbuff.id == -1){
            break;
        }

        count++;
        int id = recvbuff.id;
        int page_no = recvbuff.page_no;

        printf("Page reference (%d, %d, %d)\n",count, id, page_no);
        sprintf(buff,"Global ordering - (%d,%d,%d)\n",count,id,page_no);
        printf("%s",buff);
        write(fd, buff, strlen(buff));


        if(page_no == PROCESSCOMPLETED){
            // process finished
            sprintf(buff,"Process %d completed\n",id);
            printf("%s",buff);
            write(fd, buff, strlen(buff));
            take_away_frames(id);
            send_message_sched(TERMINATED);
        }
        else if(page_no<0 || page_no >= pinfo[id].m ){
            sprintf(buff,"Invalid page reference - (%d,%d)\n", id, page_no);
            printf("%s",buff);
            write(fd, buff, strlen(buff));
            send_message_proc(id, -1, INVALID_PAGE_REFERENCE);
            take_away_frames(id);
            send_message_sched(TERMINATED);
        }
        else{
            // printf("Before handliing\n");
            // fflush(stdout);
            handle_request(id, page_no);
            // printf("handling completed\n");
            fflush(stdout);
        }

    }

    sprintf(buff,"MMU Termination signal received\n");
    printf("%s",buff);
    write(fd, buff, strlen(buff));

    printf("Page fault count\n");
    for(int i=0;i<k;i++){
        sprintf(buff,"Process %d - %d\n",i, page_fault_count[i]);
        printf("%s",buff);
        write(fd, buff, strlen(buff));
    }
    
    freeResources();
    exit(0);
}


