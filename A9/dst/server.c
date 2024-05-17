#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>

int server_socket;

void signalhandler(int signal){
    printf("signal received: %d\n", signal);
    close(server_socket);
    exit(0);
}



int main(){
    // create a socket
    signal(SIGINT, signalhandler);
    signal(SIGTERM, signalhandler);




    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // bind the socket to local port
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(20000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
        printf("Error in binding\n");
        exit(0);
    }
    printf("Server is listening\n");

    // listen for connections
    listen(server_socket, 5);
    int client_socket;
    int i=0;
    while(1){
        client_socket = accept(server_socket, NULL, NULL);
        printf("----------------- %d -----------------\n", i++);
        printf("Client connected\n");
        // send the file
        char buff[100];
        
        int fd = open("server.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        while(recv(client_socket, buff, sizeof(buff), 0) > 0)
        {
            printf("%s",buff);
        
            
            // how to write as bytes

            write(fd, buff, strlen(buff));
            // fprintf(fp, "%s", buff);
            // fclose(fp);
        }
        close(fd);
        printf("\n");
        close(client_socket);
        
    }
    close(server_socket);
    exit(0);
}