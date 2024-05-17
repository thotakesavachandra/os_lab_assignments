#include "foothread.h"

int * childCount, * parent, *sum;
foothread_barrier_t * barrierList;
foothread_mutex_t * mutexList;
foothread_mutex_t * stdioMutex;
foothread_barrier_t * leafBarrier;


int isLeaf(int id){
    return childCount[id] == 0;
}   

int isRoot(int id){
    return parent[id] == id;
}

int isInternal(int id){
    return !isLeaf(id) && !isRoot(id);
}


int follower(void *args){

    int id = *((int*)args);

    if(isLeaf(id)){
        // it is a leaf node take input from user;
        foothread_mutex_lock(stdioMutex);
            printf("Leaf node %d :: Enter a positive integer: ",id);
            scanf("%d",&sum[id]);
            // printf("Leaf node %d :: Input = %d\n",id,sum[id]);
        foothread_mutex_unlock(stdioMutex);
        foothread_barrier_wait(leafBarrier);
    }
    else{
        // internal node. wait for all leaves to take input
        foothread_barrier_wait(leafBarrier);
    }
    
    // wait for all its children to reach the barrier
    foothread_barrier_wait(&barrierList[id]);
    
    foothread_mutex_lock(stdioMutex);
        if(isRoot(id)){    // root node
            printf("Sum at root (node %d) = %d\n",id,sum[id]);
        }
        else if(isInternal(id)){
            printf("Internal node %d gets the partial sum %d from its children\n",id,sum[id]);   
        }
    foothread_mutex_unlock(stdioMutex);

    if(isLeaf(id) || isInternal(id))
    {
        // lock the mutex for editing the parent
        foothread_mutex_lock(&mutexList[parent[id]]);
            sum[parent[id]] += sum[id];
        foothread_mutex_unlock(&mutexList[parent[id]]);

        // signal the parent
        foothread_barrier_wait(&barrierList[parent[id]]);
    }

    foothread_exit();
    return 0;
}





int main(){
    FILE * fp = fopen("tree.txt","r");

    int n;
    fscanf(fp,"%d",&n);

    childCount = (int*) malloc(sizeof(int)*n);
    parent = (int*) malloc(sizeof(int)*n);
    sum = (int*) malloc(sizeof(int)*n);
    barrierList = (foothread_barrier_t*) malloc(sizeof(foothread_barrier_t)*n);
    mutexList = (foothread_mutex_t*) malloc(sizeof(foothread_mutex_t)*n);

    stdioMutex = (foothread_mutex_t*) malloc(sizeof(foothread_mutex_t));
    foothread_mutex_init(stdioMutex);
    leafBarrier = (foothread_barrier_t*) malloc(sizeof(foothread_barrier_t));
    foothread_barrier_init(leafBarrier,n);

    for(int i=0;i<n;i++){
        int a,b;
        fscanf(fp,"%d %d",&a,&b);
        parent[a] = b;
        if(a!=b) childCount[b]++;
        sum[i] = 0;
    }   
    fclose(fp);

    for(int i=0;i<n;i++){
        foothread_barrier_init(&barrierList[i],childCount[i]+1);
        foothread_mutex_init(&mutexList[i]);
    }

    

    // create threads and process everything
    for(int i=0;i<n;i++){
        foothread_t thread;
        foothread_attr_t attr = FOOTHREAD_ATTR_INITIALIZER;
        foothread_attr_setjointype(&attr,FOOTHREAD_JOINABLE);
        int * arg = (int*) malloc(sizeof(int));
        *arg = i;
        foothread_create(&thread,&attr,follower,arg);
    }

    // sleep(60);
    foothread_exit();

    // destroy all barriers;
    foothread_mutex_destroy(stdioMutex);
    foothread_barrier_destroy(leafBarrier);

    for(int i=0;i<n;i++){
        foothread_barrier_destroy(&barrierList[i]);
        foothread_mutex_destroy(&mutexList[i]);
    }

    // free all memory

    free(childCount);
    free(parent);
    free(sum);
    free(barrierList);
    free(mutexList);
    free(stdioMutex);
    free(leafBarrier);

    return 0;

}