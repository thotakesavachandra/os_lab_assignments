#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


#define SELF 0
#define FINISHED 1
#define TERMINATED 2
#define SUSPENDED 3
#define KILLED 4
#define RUNNING 5


typedef struct Job {
    int pid;
    int pgid;
    int status;
    char name;
    struct Job* next;
}Job;

Job* newJob(Job* job,int pid, int pgid, int status, char name){
    // Job* job = (Job*)malloc(sizeof(Job));
    job->pid = pid;
    job->pgid = pgid;
    job->status = status;
    job->name = name;
    job->next = NULL;
    return job;
}

char* getName(Job* job){
    if(job->status==SELF){
        return "mgr";
    }
    char * name = (char*)malloc(sizeof(char)*100);
    sprintf(name,"job %c",job->name);
    return name;
}

char* getStatus(Job* job){
    int i = job->status;
    switch(i){
        case 0:
            return "SELF";
        case 1:
            return "FINISHED";
        case 2:
            return "TERMINATED";
        case 3:
            return "SUSPENDED";
        case 4:
            return "KILLED";
        case 5:
            return "RUNNING";
    }
}


/************************************** GLOBAL VARIABLES **********************************************/
// Job* head;
// Job* tail;
Job jobs[11];
int jobCount = 0;
int isScanf = 0;

/******************************************************************************************************/

void signalHandler ( int sig )
{
    printf("\n");
    // printf("isca : %d\n",isScanf);
    if(isScanf) printf("mgr> ");
    fflush(stdout);

    int temp = 0;
    while(temp<=jobCount){
        if(jobs[temp].status==RUNNING){
            break;
        }
        temp++;
    }
    if(temp>jobCount) return;
    
    if(sig==SIGINT){
        kill(jobs[temp].pid,SIGINT);
        jobs[temp].status = TERMINATED;
    }
    else if(sig==SIGTSTP){
        // printf("Suspend signal\n");
        kill(jobs[temp].pid,SIGTSTP);
        jobs[temp].status = SUSPENDED;
    }
}



void printer(){
    printf("\t%-12s %-12s %-12s %-12s %-12s\n","NO","PID","PGID","STATUS","NAME");
    for(int i=0;i<=jobCount;i++){
        printf("\t%-12d %-12d %-12d %-12s %-12s\n",i,jobs[i].pid,jobs[i].pgid,getStatus(&jobs[i]),getName(&jobs[i]));
    }
}

void cleanUp(){
    for(int i=0;i<=jobCount;i++){
        if(jobs[i].status==SUSPENDED){
            kill(jobs[i].pid,SIGINT);
        }
    }
}

void runNewJob(){
    if(jobCount==10){
        printf("Process table is full. Quiting...\n");
        cleanUp();
        exit(1);
    }
    char name = 'A'+rand()%26;
    int status = RUNNING;
    int pid = fork();
    
    if(pid==0){     // child process
        char arg[2];
        arg[0] = name;
        arg[1] = '\0';
        execlp("./job","job",arg,NULL);
    }
    else{           // parent process
        jobCount++;
        int pgid = setpgid(pid,pid);
        pgid = __getpgid(pid);
        newJob(&jobs[jobCount],pid,pgid,status,name);
        
        int runStatus;
        waitpid(pid,&runStatus,WUNTRACED);
        // printf("checking for child\n");
        // printf("isComp : %d,runStatus : %d\n",isComp,runStatus);
        if(runStatus==2){
            // printf("Process Terminated\n");
        }
        if(runStatus==5247){
            // jobs[jobCount].status = SUSPENDED;
            // printf("Process Suspended\n");
        }
        if(runStatus==0){
            jobs[jobCount].status = FINISHED;
            // printf("Process Finished\n");
        }
            
    }
}

int getChoice(){
    int temp = 0;
    int nSuspended = 0;
    int list[10];
    while(temp<=jobCount){
        if(jobs[temp].status==SUSPENDED){
            list[nSuspended] = temp;
            nSuspended++;
        }
        temp++;
    }
    if(nSuspended==0) return 0;
    printf("Suspended Jobs : ");
    for(int i=0;i<nSuspended;i++){
        printf("%d ,",list[i]);
    }
    printf(" (Pick One) : ");
    int choice;
    scanf("%d",&choice);
    if(choice<1 || choice>jobCount || jobs[choice].status!=SUSPENDED) return 0;
    return choice;
}


void continueJob(){
    int choice = getChoice();
    if(choice==0){
        // printf("Invalid Choice\n");
        return;
    }
    kill(jobs[choice].pid,SIGCONT);
    jobs[choice].status = RUNNING;
    int pid = jobs[choice].pid;
    int runStatus;
    waitpid(pid,&runStatus,WUNTRACED);
    if(runStatus==2){
        // printf("Process Terminated\n");
        // jobs[choice].status = TERMINATED;
    }
    if(runStatus==5247){
        // printf("Job %d suspended\n",choice);
        // jobs[choice].status = SUSPENDED;
    }        
    if(runStatus==0){
        // printf("Process Finished\n");
        jobs[choice].status = FINISHED;
    }    
    
}

void killJob(){
    int choice = getChoice();
    if(choice==0){
        // printf("Invalid Choice\n");
        return;
    }
    kill(jobs[choice].pid,SIGINT);
    jobs[choice].status = KILLED;
}

void getHelp(){
    printf("%10s : %s\n","Command","Action");
    printf("%10s : %s\n","c","Continue a suspended job");
    printf("%10s : %s\n","h","Print this help message");
    printf("%10s : %s\n","k","Kill a suspended job");
    printf("%10s : %s\n","p","Print the process table");
    printf("%10s : %s\n","q","Quit");
    printf("%10s : %s\n","r","Run a new job");
}



void quit(){
    cleanUp();
    exit(0);
}


int main ()
{
    jobCount = 0;
    newJob(&jobs[0],getpid(),getpgid(getpid()),SELF,' ');
    srand((unsigned int)time(NULL));
    signal(SIGINT,signalHandler);
    signal(SIGTSTP,signalHandler);

    char op[2];
    while(1){
        printf("mgr> ");
        isScanf = 1;
        scanf("%s",op);
        isScanf = 0;
        getchar();
        if(op[0]=='p'){
            printer();
        }
        if(op[0]=='r'){
            runNewJob();
        }
        if(op[0]=='c'){
            continueJob();
        }
        if(op[0]=='k'){
            killJob();
        }
        if(op[0]=='h'){
            getHelp();
        }
        if(op[0]=='q'){
            quit();
        }

    }
   
}
