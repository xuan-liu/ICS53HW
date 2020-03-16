// name: Xuan Liu, student ID: 35689116
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <stdint.h> 
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 

#define MAXLINE 256

void respond(int connfd);

/* create a listening descriptor that can be used to accept connection requests from clients */
int open_listenfd(char *port){
    int serverfd;
    int opt = 1;

    // Creating socket file descriptor
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket failed");
        return -1;
    }

    // Forcefully attaching socket to the port 8080
    // Set socket options
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEPORT, (const void*)&opt, sizeof(opt))){
        perror("setsockopt");
        return -1;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, '0', sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(8080);

    // Forcefully attaching socket to the port 8080
    if (bind(serverfd, (struct sockaddr *)&client_addr, sizeof(client_addr))<0){
        perror("bind failed");
        return -1;
    }

    // listen
    if (listen(serverfd, 3) < 0){
        perror("listen");
        return -1;
    }
    return serverfd;
}

int main(int argc, char **argv){
    //print out initial instructions
    printf("server started\n");

    char message[MAXLINE]; //buffer to hold payload received and sent

    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    listenfd = open_listenfd(argv[1]);
    while(1) {
        clientlen = sizeof(struct sockaddr_storage);
        if ((connfd = accept(serverfd, (SA *)&clientaddr, &clientlen))<0){
            perror("accept");
            return -1;
        }
        getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);

        respond(connfd);
        close(connfd);
    }
    exit(0);
}

/* the server reads the request and print the game_id and field in the message, also send 
the value of that field for the corresponding match response to the client */
void respond(int connfd){

}