/*************************

CS39002 Operating Systems Laboratory
Assignment 1
Thota Kesava Chandra (21CS30056)

*************************/



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char* argv[])
{
    // checking for the city argument
    if(argc==1){
        printf("Run with a node name\n");
        exit(1);
    }


    // opening the text file for information
    FILE* fptr = fopen("./treeinfo.txt","r");
    if(fptr==NULL){
        printf("Tree File not found\n");
        exit(1);
    }


    // reading the file for the city
    char city[100];
    int nchild = 0;
    int found = 0;
    while(1){
        if(fscanf(fptr,"%s %d",city,&nchild)<=0) break;
        //fscanf(fptr,"%s %d",city,&nchild);
        if(strcmp(city,argv[1])==0){
            found = 1;
            break;
        }
        char dump[100];
        while(nchild--){
            fscanf(fptr,"%s",dump);
        }
    }
    if(found==0){
        printf("City %s not found\n",argv[1]);
        exit(1);
    }


    // reading and storing children of the city
    char** children = (char**)malloc(nchild*sizeof(char*));
    for(int i=0;i<nchild;i++){
        children[i] = (char*)malloc(100*sizeof(char));
        fscanf(fptr,"%s",children[i]);
        //printf("%s\n", children[i]);
    }

    fclose(fptr);


    // printing the city name
    int indent = 0;
    if(argc>2) indent = atoi(argv[2]);

    for(int i=0;i<indent;i++) printf("    ");
    printf("%s (%d)\n",city,getpid());
    fflush(stdout);


    // calling the child processes
    for(int i=0;i<nchild;i++){
        pid_t x = fork();
        if(x==0){
            char newindent[10];
            sprintf(newindent,"%d",indent+1);
            execlp("./proctree","proctree",children[i],newindent,NULL);
        }
        else{
            wait(NULL);
        }
    }

}
