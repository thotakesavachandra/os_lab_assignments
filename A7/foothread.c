#include "foothread.h"

static int __isInitiated = 0;
static int globalMutexId;

typedef struct thread_list_t{
    int tid;
    foothread_barrier_t barrier;
    struct thread_list_t* next;
}thread_list_t;

static thread_list_t* listHead = NULL;
static thread_list_t* listTail = NULL;


void insert_into_thread_list(int tid){
    if(listHead==NULL){
        listHead = (thread_list_t*) malloc(sizeof(thread_list_t));
        listTail = listHead;
        thread_list_t* temp = listHead;
        temp->tid = tid;
        foothread_barrier_init(&(temp->barrier),2);
        return;
    }
    thread_list_t* temp = (thread_list_t*) malloc(sizeof(thread_list_t));
    temp->tid = tid;
    foothread_barrier_init(&(temp->barrier),2);
    listTail->next = temp;
    listTail = temp;
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

// creates Resources for the library to function
// void foothread_library_init(){
//     // create global mutex
//     globalMutexId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
//     semctl(globalMutexId, 0, SETVAL, 1);

//     exitInitSemId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
//     semctl(exitInitSemId, 0, SETVAL, 0);

//     __nJoined = 0;
//     exitBarrier = (foothread_barrier_t*) malloc(sizeof(foothread_barrier_t));
// }

// // Frees all the resources the library has used
// void foothread_library_exit(){
//     semctl(globalMutexId, 0, IPC_RMID);
//     semctl(exitInitSemId, 0, IPC_RMID);
//     foothread_barrier_destroy(exitBarrier);
//     free(exitBarrier);

// }

void foothread_create(foothread_t *thread, foothread_attr_t *attr, int (*start_routine)(void * ), void *arg){
    if(!__isInitiated){
        __isInitiated = 1;
        globalMutexId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        semctl(globalMutexId, 0, SETVAL, 1);
    }

    char* stack = (char*) malloc(FOOTHREAD_DEFAULT_STACK_SIZE);

    thread->stack = stack + attr->stack_size;
    thread->stack_size = attr->stack_size;
    thread->is_joinable = attr->is_joinable;

    int flags = CLONE_THREAD | CLONE_VM | CLONE_SIGHAND;
    thread->tid = clone(start_routine, thread->stack, flags, arg);
    
    if(attr->is_joinable==FOOTHREAD_JOINABLE){
        insert_into_thread_list(thread->tid);
    }
}

void foothread_attr_setjointype(foothread_attr_t *attr, int type){
    attr->is_joinable = type;
    return;
}
void foothread_attr_setstacksize(foothread_attr_t *attr, int size){
    attr->stack_size = size;
    return;
}

// leader waits for all barriers and child waits for its own barrier
void foothread_exit (){
    if(listHead==NULL) return;  // not needed explicitly but still

    if(gettid()!=getpid()){ // this is a child thread
        thread_list_t* temp = listHead;
        while(temp!=NULL){
            if(temp->tid == gettid()){
                break;
            }
            temp = temp->next;
        }
        if(temp==NULL) return;
        foothread_barrier_wait(&(temp->barrier));
    }
    else{      // this is parent thread
        thread_list_t* temp = listHead;
        while(temp!=NULL){
            foothread_barrier_wait(&(temp->barrier));
            temp = temp->next;
        }
        thread_list_t* curr = listHead;
        thread_list_t* next;
        while(curr!=NULL){
            next = curr->next;
            foothread_barrier_destroy(&(curr->barrier));
            free(curr);
            curr = next;
        }
    }
}




void foothread_mutex_init ( foothread_mutex_t *mutex ) {
    wait_sem(globalMutexId,0,1);
        mutex->semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        mutex->editId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if(mutex->semId == -1 || mutex->editId == -1){
            perror("semget");
            signal_sem(globalMutexId,0,1);
            exit(1);
        }

        semctl(mutex->semId, 0, SETVAL, 1);
        semctl(mutex->editId, 0, SETVAL, 1);
        mutex->state = UNLOCKED;
        mutex->tid = -1;
    signal_sem(globalMutexId,0,1);
}




void foothread_mutex_lock ( foothread_mutex_t *mutex ) {
    wait_sem(mutex->editId,0,1);
        if(mutex->state == UNUSUABLE){
            perror("Mutex is unusable");
            signal_sem(globalMutexId,0,1);
            exit(1);
        }
    signal_sem(mutex->editId,0,1);
 
    wait_sem(mutex->semId,0,1);

    wait_sem(mutex->editId,0,1);

        mutex->state = LOCKED;
        mutex->tid = gettid();

    signal_sem(mutex->editId,0,1);
    return;
}

void foothread_mutex_unlock ( foothread_mutex_t *mutex ) {
    wait_sem(mutex->editId,0,1);
        if(mutex->state == UNUSUABLE){
            perror("Mutex is unusable");
            signal_sem(mutex->editId,0,1);
            exit(1);
        }
        if(mutex->state==UNLOCKED){
            perror("Mutex is already unlocked");
            signal_sem(mutex->editId,0,1);
            exit(1);
        }
        if(mutex->tid != gettid()){
            perror("Mutex is not locked by this thread");
            signal_sem(mutex->editId,0,1);
            exit(1);
        }
        signal_sem(mutex->semId,0,1);
        mutex->state = UNLOCKED;
        mutex->tid = -1;
    signal_sem(mutex->editId,0,1);
    return;
}

void foothread_mutex_destroy ( foothread_mutex_t *mutex ) {
    wait_sem(mutex->editId,0,1);
        if(mutex->state == UNUSUABLE){
            perror("Mutex is unusable");
            signal_sem(mutex->editId,0,1);
            exit(1);
        }
        mutex->state = UNUSUABLE;
        semctl(mutex->semId, 0, IPC_RMID);
        semctl(mutex->editId, 0, IPC_RMID);
    return;
}


void foothread_barrier_init ( foothread_barrier_t *barrier , int count ) {
    wait_sem(globalMutexId,0,1);
        barrier->semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        barrier->editId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
        if(barrier->semId == -1){
            perror("semget");
            signal_sem(globalMutexId,0,1);
            exit(1);
        }
        semctl(barrier->semId, 0, SETVAL, 0);
        semctl(barrier->editId, 0, SETVAL, 1);
        barrier->count = count;
        barrier->state = UNLOCKED;
    signal_sem(globalMutexId,0,1);
}

void foothread_barrier_wait ( foothread_barrier_t *barrier ) {
    wait_sem(barrier->editId,0,1);
        if(barrier->state == UNUSUABLE){
            signal_sem(barrier->editId,0,1);
            perror("Barrier is unusable");
            exit(1);
        }
        signal_sem(barrier->semId,0,1);

    signal_sem(barrier->editId,0,1);
    
    // printf("barrier wait %d=>%d\n",gettid(),barrier->semId);
    wait_sem(barrier->semId,0,barrier->count);

    wait_sem(barrier->editId,0,1);
        signal_sem(barrier->semId,0,barrier->count);
    signal_sem(barrier->editId,0,1);

    return;
}

void foothread_barrier_destroy ( foothread_barrier_t *barrier ) {
    wait_sem(barrier->editId,0,1);
        if(barrier->state == UNUSUABLE){
            perror("Barrier is unusable");
            signal_sem(barrier->editId,0,1);
            exit(1);
        }
        barrier->state = UNUSUABLE;
        semctl(barrier->semId, 0, IPC_RMID);
        semctl(barrier->editId, 0, IPC_RMID);
    return;
}


