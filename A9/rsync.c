#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ftw.h>
#include <utime.h>

#define SRC_DIR 0
#define DST_DIR 1


char* src_path;
char* dst_path;

char* new_path;


int getMaxSize(const char* buff){
    return strlen(src_path)+strlen(dst_path)+strlen(buff)+1;
}

void get_other_path(char* buff, int target){
    int len = getMaxSize(buff);
    char newbuff[len];
    if(target==SRC_DIR){
        strcpy(newbuff,src_path);
        char* rptr = buff + strlen(dst_path);
        strcat(newbuff, rptr);
    }
    else{
        strcpy(newbuff,dst_path);
        char* rptr = buff + strlen(src_path);
        strcat(newbuff, rptr);
    }
    // printf("buffer : %s\n",buff);
    // printf("newbuff : %s\n",newbuff);
    strcpy(buff, newbuff);
}



// challenge!! how to recursively delete a file structure//
// use nftw to recursively check from the bottom of the file
// if directory compare only the names 
// compare the stat of the src file with dst file. in case of any error remove the file
int count = 0;

int rem_check(const char* path, const struct stat* info, int flag, struct FTW*){
    count ++;
    char other_path[getMaxSize(path)];
    strcpy(other_path, path);
    get_other_path(other_path, SRC_DIR);
    // printf("path: %s\n",path);
    // printf("other_path: %s\n",other_path);

    int marker = 0;

    struct stat other_info;
    if(stat(other_path, &other_info) == -1){
        // file is deleted in the source dir
        // remove(path);
        // needs to remove the file
        marker = 1;
    }

    else if(S_ISDIR(info->st_mode)){

        if(S_ISDIR(other_info.st_mode)){
            // do nothing

        }
        else{
            // remove this file
            
            // remove(path);
            marker = 1;
        }

    }
    
    else{

        if(S_ISDIR(other_info.st_mode)){
            // remove this file
            // remove(path);
            marker = 1;
        }
        // else{
        //     // check for size and timestamp of both files if not matched then delete
        //     if(info->st_size != other_info.st_size || info->st_mtime != other_info.st_mtime){
        //         // overwriting case
        //         remove(path);
        //     }

        // }

    }
    if(marker == 1){
        remove(path);
        printf("[-] %-80s [item is deleted from the destination]\n", path);
    }
    return 0;
}

void create_and_copy(const char* src, const char* dst){
    FILE* read_fp = fopen(src,"r");
    FILE* write_fp = fopen(dst,"w");

    if (read_fp == NULL || write_fp == NULL) {
        printf("Error opening file\n");
        printf("%s || %s\n",src,dst);
        return;
    }

    char buff[1000];
    size_t bytesRead;

    while ((bytesRead = fread(buff, 1, sizeof(buff), read_fp)) > 0) {
        fwrite(buff, 1, bytesRead, write_fp);
    }

    fclose(read_fp);
    fclose(write_fp);
}


int create_check(const char* path, const struct stat* info, int flag, struct FTW*){
    count ++;
    char other_path[getMaxSize(path)];
    strcpy(other_path, path);
    get_other_path(other_path, DST_DIR);
    // printf("path: %s\n",path);
    // printf("other_path: %s\n",other_path);

    struct stat other_info;
    if(stat(other_path, &other_info) == -1){
        // file is not present in the destn
        if(S_ISDIR(info->st_mode)){
            // mkdir the directory
            mkdir(other_path, info->st_mode);
        }
        else{
            // create the file
            create_and_copy(path, other_path);
        }
        printf("[+] %-80s [item is added to the destination]\n", other_path);
    }
    else if(S_ISDIR(info->st_mode)){
        // this is a directory other path must be directory do nothing

    }
    else if(info->st_size != other_info.st_size || info->st_mtime != other_info.st_mtime){
        // overwriting case
        remove(other_path);
        create_and_copy(path, other_path);
        printf("[o] %-80s [item in destination is overwritten by item in source]\n", other_path);
    }

    // get the status
    stat(other_path, &other_info);

    // copy the status of the file from src to dst
    if(info->st_atime != other_info.st_atime || info->st_mtime != other_info.st_mtime){
        struct utimbuf time_buff;
        time_buff.actime = info->st_atime;
        time_buff.modtime = info->st_mtime;
        utime(other_path, &time_buff);
        printf("[t] %-80s [timestamp changed for item in destination]\n", other_path);
    }

    if(info->st_mode != other_info.st_mode){
        // change permissoins
        chmod(other_path, info->st_mode);
        printf("[p] %-80s [permission changed for item in destination]\n", other_path);
    }

    
    return 0;
}



int main(){
    src_path = "./src";
    dst_path = "./dst";


    int ret = nftw(dst_path, rem_check, 20, FTW_DEPTH);
    // printf("count : %d, ret = %d\n",count,ret);

    ret = nftw(src_path, create_check, 20, 0);



    // new_path = "/mnt/Study/Kesava/Sem 6/OS/A9/hehe";
    // dst_path = "/Desktop";
    // char* cwd = getcwd(NULL,0);
    
    // ftw(cwd, func, 0);

}