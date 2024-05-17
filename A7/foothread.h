#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/sem.h>
#include <sched.h>
#include <sys/mman.h>
#include <err.h>
#include <errno.h>


// void foothread_library_init();
// void foothread_library_exit();


#define FOOTHREAD_DEFAULT_STACK_SIZE 2097152

typedef struct foothread_t{
    int tid;
    int leader_tid;
    int is_joinable;
    int stack_size;
    void* stack;
}foothread_t;



#define FOOTHREAD_DETACHED 0
#define FOOTHREAD_JOINABLE 1

typedef struct foothread_attr_t{
    int stack_size;
    int is_joinable;
}foothread_attr_t;

#define FOOTHREAD_ATTR_INITIALIZER {FOOTHREAD_DEFAULT_STACK_SIZE,0}


void foothread_create ( foothread_t * , foothread_attr_t * , int (*)(void *) , void * ) ;
void foothread_attr_setjointype ( foothread_attr_t * , int ) ;
void foothread_attr_setstacksize ( foothread_attr_t * , int ) ;
void foothread_exit ( ) ;

#define UNUSUABLE -1
#define UNLOCKED 1
#define LOCKED 0


typedef struct foothread_mutex_t{
    int state;
    int semId;
    int tid;
    int editId;
} foothread_mutex_t;

void foothread_mutex_init ( foothread_mutex_t * ) ;
void foothread_mutex_lock ( foothread_mutex_t * ) ;
void foothread_mutex_unlock ( foothread_mutex_t * ) ;
void foothread_mutex_destroy ( foothread_mutex_t * ) ;



typedef struct foothread_barrier_t{
    int state;
    int count;
    int semId;
    int editId;
} foothread_barrier_t;


void foothread_barrier_init ( foothread_barrier_t * , int ) ;
void foothread_barrier_wait ( foothread_barrier_t * ) ;
void foothread_barrier_destroy ( foothread_barrier_t * ) ;