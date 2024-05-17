#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


void parent(){
    printf("+++ CSE in supervisor mode: Started\n");

    int fdAtoB[2];
    pipe(fdAtoB);
    char fdAtoBPass[2][10];
    sprintf(fdAtoBPass[0],"%d",fdAtoB[0]);
    sprintf(fdAtoBPass[1],"%d",fdAtoB[1]);

    int fdBtoA[2];
    pipe(fdBtoA);
    char fdBtoAPass[2][10];
    sprintf(fdBtoAPass[0],"%d",fdBtoA[0]);
    sprintf(fdBtoAPass[1],"%d",fdBtoA[1]);
    


    printf("+++ CSE in supervisor mode: pfd AtoB = [%d %d] , pfd BtoA = [%d %d]\n",fdAtoB[0],fdAtoB[1],fdBtoA[0],fdBtoA[1]);
    if(fork()==0){
        close(fdAtoB[0]);
        close(fdBtoA[1]);
        execlp("xterm","xterm","-T","First Child", "-e", "./CSE","1",fdBtoAPass[0],fdAtoBPass[1],NULL);
        // execlp("./CSE","CSE","1",fdPass[0],fdPass[1],NULL);
    }
    printf("+++ CSE in supervisor mode: Forking first child in command-input mode\n");


    if(fork()==0){
        close(fdAtoB[1]);
        close(fdBtoA[0]);
        execlp("xterm","xterm","-T","Second Child", "-e", "./CSE","2",fdAtoBPass[0],fdBtoAPass[1],NULL);
        // execlp("./CSE","CSE","2",fdPass[0],fdPass[1],NULL);
    }
    printf("+++ CSE in supervisor mode: Forking second child in execute mode\n");
    wait(NULL);
    wait(NULL);
}

char** getArgs(char command[100]){
    char** args = (char**)malloc(10*sizeof(char*));
    int i=0;
    int count = 0;
    int len = 0;
    while(command[len]!='\n'){
        len++;
    }

    while(i<len){
        int j = i;
        while(command[j]!=' ' && command[j]!='\n'){
            j++;
        }
        args[count] = (char*)malloc((j-i+1)*sizeof(char));
        int k=0;
        while(i<j){
            args[count][k] = command[i];
            i++;
            k++;
        }
        args[count][k] = '\0';
        // printf("%s\n",args[count]);
        // fflush(stdout);
        count++;
        i = j+1;
    }
    // fflush(stdout);
    args[count] = NULL;
    // sleep(20);
    return args;
}


int main(int argc, char* argv[]){
    if(argc<3){
        parent();
    }  
    else{
        int mode = atoi(argv[1]);
        int fd[2];
        fd[0] = atoi(argv[2]);
        fd[1] = atoi(argv[3]);
        printf("reads from %d and writes to %d\n",fd[0],fd[1]);
        int newStdin = dup(0);
        int newStdout = dup(1);
        int t=10;
        while(t--){
            if(mode==1){
                dup2(newStdin,0);
                dup2(fd[1],1);
                write(2,"Command> ",10);
                // get line from user
                char command[100];
                
                fgets(command, 100, stdin);
                printf("%s",command);
                fflush(stdin);
                fflush (stdout);
                if(strcmp(command,"exit\n")==0){
                    printf("+++ CSE in command-input mode: Exiting\n");
                    exit(0);
                }
                if(strcmp(command,"swaprole\n")==0){
                    mode=2;
                }
            }
            else{
                dup2(fd[0],0);
                dup2(newStdout,1);
                char command[100];
    
                printf("Waiting for Command> ");
                fflush(stdout);
                fgets(command, 100, stdin);
                printf("%s",command);
                if(strcmp(command,"exit\n")==0){
                    printf("+++ CSE in execute mode: Exiting\n");
                    exit(0);
                }
                if(strcmp(command,"swaprole\n")==0){
                    mode=1;
                }
                
                char ** args = getArgs(command);


                // fflush(stdout);
                if(fork()==0){
                    dup2(newStdin,0);
                    dup2(newStdout,1);
                    if(execvp(args[0],args)<0) printf("+++ CSE in execute mode: Command not found\n");
                    exit(0);
                }
                else{
                    wait(NULL);
                }

                // sleep(20);
            }
        }
    }
}