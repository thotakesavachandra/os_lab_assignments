#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>
#include <pthread.h>
#include <unistd.h>
#include <bits/pthreadtypes.h>


#define REPORTER 0
#define PATIENT 1
#define SALESREP 2

// global variables 
char names[3][20] = {"Reporter","Patient","Sales rep"};
int wait_vis[3];
int max_vis[3];
int token_vis[3];
int curr_time,last_time;
int isSessionDone,printed_exit;
pthread_barrier_t init_barrier,doc_barrier,done_doc,done_vis;
pthread_mutex_t mutex;
pthread_cond_t cond_vis[3],cond_doc,init_vis;


typedef struct {
    int type;
    int num;
    int time;
    int duration;
} vinfo;

void getTime(int ti,char t[10]){
    ti += 9*60;
    int hour = ti/60;
    int min = ti%60;
    if(hour>12){
        sprintf(t,"%02d:%02d",hour-12,min);
    }
    else{
        sprintf(t,"%02d:%02d",hour,min);
    }
    if(hour>=12) strcat(t," PM");
    else strcat(t," AM");
}

void init(){
    isSessionDone = 0;
    printed_exit = 0;

    last_time = 0;
    curr_time = 0;

    wait_vis[REPORTER] = 0;
    wait_vis[PATIENT] = 0;
    wait_vis[SALESREP] = 0;

    max_vis[REPORTER] = 1e9;
    max_vis[PATIENT] = 25;
    max_vis[SALESREP] = 3;

    token_vis[REPORTER] = 1;
    token_vis[PATIENT] = 1;
    token_vis[SALESREP] = 1;

    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_trylock(&mutex);
    pthread_mutex_unlock(&mutex);
    

    pthread_cond_init(&cond_doc,NULL);
    
    pthread_cond_init(&init_vis,NULL);

    for(int i=0;i<3;i++){
        pthread_cond_init(&cond_vis[i],NULL);
    
    }
}

void print_consultation_details(int type,int num,int time,int duration){
    char st[10],end[10];
    getTime(time,st);
    getTime(time+duration,end);

    printf("🔵[%s-%s] %s %d is in doctor's chamber\n",st,end,names[type],num);
}


void* visitor( void* args ){
    vinfo *params = (vinfo*)args;

    int type = params->type;
    int num = params->num;
    int duration = params->duration;
    
    
    pthread_mutex_lock(&mutex);
        pthread_barrier_wait(&init_barrier);
        pthread_cond_wait(&cond_vis[type],&mutex);
        
        last_time = curr_time+duration;
        print_consultation_details(type,num,curr_time,duration);
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&done_vis);
    // get_consultation();
    pthread_exit(NULL);
}

void* doctor( void* args ){
    while(1){
        pthread_mutex_lock(&mutex);
            pthread_barrier_wait(&doc_barrier);
            pthread_cond_wait(&cond_doc,&mutex);
            if(isSessionDone){
                char t[10]; getTime(curr_time,t);
                printf("🔴[%s] Doctor leaves\n",t);
                pthread_mutex_unlock(&mutex);
                break;
            }
            char t[10]; getTime(curr_time,t);
            printf("🔴[%s] Doctor has next visitor\n",t);
        pthread_mutex_unlock(&mutex);
        
        // consult();
    }
    pthread_barrier_wait(&doc_barrier);
    pthread_exit(NULL);
}

void getSync(pthread_barrier_t* barrier){
    pthread_barrier_wait(barrier);
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
}

void signalDoctor(){

    pthread_barrier_init(&doc_barrier, NULL, 2);
    pthread_cond_signal(&cond_doc);


    getSync(&doc_barrier);
    pthread_barrier_destroy(&doc_barrier);
}

void signalVisitor(int type){
    pthread_barrier_init(&done_vis, NULL, 2);
    pthread_cond_signal(&cond_vis[type]);
    pthread_barrier_wait(&done_vis);
    pthread_barrier_destroy(&done_vis);
}

int getType(char c){
    if(c == 'R') return REPORTER;
    else if(c == 'P') return PATIENT;
    else if(c == 'S') return SALESREP;
    return -1;
}

void create_thread(event e){
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    vinfo *params = (vinfo*)malloc(sizeof(vinfo));
    params->type = getType(e.type);
   
    params->num = token_vis[params->type];
    params->time = curr_time;
    params->duration = e.duration;
    pthread_barrier_init(&init_barrier, NULL, 2);

    token_vis[params->type]++;
    wait_vis[params->type]++;

    
    
    pthread_create(&tid,&attr,visitor,(void*)params);

    getSync(&init_barrier);
    pthread_barrier_destroy(&init_barrier); 
}

void printArrival(event e){
    char t[10];
    getTime(e.time,t);
   
    int type = getType(e.type);
    printf("\t⚪[%s] %s %d arrives\n",t,names[type],token_vis[type]);


}


int checkSession(){
    if(token_vis[PATIENT] > max_vis[PATIENT] && token_vis[SALESREP] > max_vis[SALESREP] && wait_vis[SALESREP] == 0 &&
                wait_vis[PATIENT] == 0 && wait_vis[REPORTER] == 0){
        return 1;
    }
    return 0;

}


void checkAndCreateThread(event e){
    int type = getType(e.type);
    char msg[2][30];
    strcpy(msg[0],"quota full");
    strcpy(msg[1],"session over");
    int choice = 0;

    if(!printed_exit && token_vis[type] <= max_vis[type]){
        if(e.time > last_time){
            last_time = e.time;
        }
        create_thread(e);
        return;
    }
    if(printed_exit){
        choice = 1;
    }

    char t[10];
    getTime(e.time,t);
    printf("\t⚪[%s] %s %d leaves (%s)\n",t,names[type],token_vis[type],msg[choice]);
    token_vis[type]++;
}



int main(){
    init();

    eventQ E;
    E = initEQ("arrival.txt");
    pthread_t doctor_tid;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_barrier_init(&doc_barrier, NULL, 2);   
    pthread_create(&doctor_tid, &attr, doctor, NULL);
    getSync(&doc_barrier);
    pthread_barrier_destroy(&doc_barrier);
    

    while(1){

        while(!emptyQ(E)){
            event e = nextevent(E);
            if(e.time <= last_time){
                printArrival(e);
                checkAndCreateThread(e);
            }
            else{
                break;
            }
            E = delevent(E);

        }


        isSessionDone = checkSession();
        if(isSessionDone && !printed_exit){
            curr_time = last_time;
            signalDoctor();
            printed_exit = 1;
        }
        
        if(wait_vis[REPORTER] > 0){
            curr_time = last_time;
            
            wait_vis[REPORTER]--;
            signalDoctor();
            signalVisitor(REPORTER);
            
        }
        else if(wait_vis[PATIENT] > 0){
            curr_time = last_time;
            
            wait_vis[PATIENT]--;
            signalDoctor();
           
            signalVisitor(PATIENT);
        }
        else if(wait_vis[SALESREP] > 0){
            curr_time = last_time;
            
            wait_vis[SALESREP]--;
            signalDoctor();
            signalVisitor(SALESREP);
        }
        else{
            if(emptyQ(E)) break;
            event e = nextevent(E);
            last_time = e.time;

        }
        
    }
    
    if(isSessionDone && !printed_exit){
        curr_time = last_time;
        signalDoctor();
        printed_exit = 1;
    }

}
