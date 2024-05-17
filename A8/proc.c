#include "includes.h"

int no_of_pages = 0;
int pg_no[1000];
int ready_id, proc_mmu_id;
struct msgbuff sendbuff, recvbuff;
// int id;

void conv_ref_pg_no(char * refs)
{
	const char s[2] = "|";
	char *token;
	token = strtok(refs, s);
	while ( token != NULL )
	{
		pg_no[no_of_pages] = atoi(token);
		no_of_pages++;
		token = strtok(NULL, s);
	}
}

void send_msg_mmu(int id, int page_no){
    // printf("\t\tsending messge with id=%d, page_no=%d\n",id,page_no);
    sendbuff.mtype = MMUDESTN;
    sendbuff.id = id;
    sendbuff.page_no = page_no;
    sendbuff.frame_no = -1;
    int rst = msgsnd(proc_mmu_id, &sendbuff, sizeof(struct msgbuff), 0);
    if(rst==-1){
        perror("msgsnd");
        exit(0);
    }

}


int main(int argc, char* argv[])
{
    if(argc!=5){
        printf("\t\tUsage: ./proc <ready_id> <proc_mmu_id> <page_references>\n");
        exit(0);
    }
    
    int id = atoi(argv[1]);
    ready_id = atoi(argv[2]);
    proc_mmu_id = atoi(argv[3]);
    conv_ref_pg_no(argv[4]);

    printf("\t\tProcess %d spawned\n",id);
    printf("\t\tId:%d pid:%d\n",id,getpid());
    
    // add itself to the queue
    {
        sendbuff.mtype = SCHEDDESTN;
        sendbuff.id = id;
        sendbuff.page_no = -1;
        sendbuff.frame_no = -1;
        int rst = msgsnd(ready_id, &sendbuff, sizeof(struct msgbuff), 0);
    }

    printf("\t\tProcess (%d,%d) added to queue\n",id,getpid());

    // wait for initial message from scheduler
    int store_id = id;
    int rst = msgrcv(ready_id, &recvbuff, sizeof(struct msgbuff), id+BASEID, 0);
    if(rst==-1){
        perror("msgrcv");
        exit(0);
    }
    


    int ptr = 0;
    while(ptr<no_of_pages){
        printf("\t\tProc:(%d,%d) requesting page %d\n",id,getpid(),pg_no[ptr]);
        send_msg_mmu(id, pg_no[ptr]);
        rst = msgrcv(proc_mmu_id, &recvbuff, sizeof(struct msgbuff), id+BASEID, 0);
        
        if(rst==-1){
            perror("msgrcv");
            exit(0);
        }
        if(recvbuff.frame_no==PAGE_FAULT_OCCURED){
            // page fault wait for scheduler to add to queue
            printf("\t\tProc:(%d,%d) Page fault occured\n", id, getpid());
            rst = msgrcv(ready_id, &recvbuff, sizeof(struct msgbuff), id+BASEID, 0);
            if(rst==-1){
                perror("msgrcv");
                exit(0);
            }
        }
        else if(recvbuff.frame_no==INVALID_PAGE_REFERENCE){
            // process should be terminated
            printf("\t\tProc:(%d,%d) Invalid page reference\n", id, getpid());
            exit(1);
        }
        else{
            printf("\t\tframe received (%d,%d) : (%d=>%d)\n",id, getpid(), pg_no[ptr], recvbuff.frame_no);
            ptr++;
        }

    }

    // process completed
    printf("\t\tProcess %d completed and exiting\n",id);    
    send_msg_mmu(id, PROCESSCOMPLETED);
    
}