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
const char* prompt = "> ";

/* establish a connection with a server */
int open_clientfd(char *hostname, char *port) {
    int clientfd;

    // Create socket descriptor
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // connect to the server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (connect(clientfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\nConnection Failed \n");
        return -1;
    }
    return clientfd;
}

int main(int argc, char **argv){
    //print out initial instructions
    printf("client start\n");

    int clientfd;
    char *host = argv[1];
    char *port = argv[2];
    char cmdline[MAXLINE];  // orginal input
    char output[MAXLINE]; // message received from server
    char input[MAXLINE]; // message send from client to server
    char response[MAXLINE]; // print form of output

    //connect
    clientfd = open_clientfd(host, port);

    // user interface of the client
    while (1) {
        // print a prompt
        printf("%s", prompt);

        // read cmdline
        fgets(cmdline, MAXLINE, stdin);
        cmdline[strcspn(cmdline, "\n")] = 0;
        
        // if cmdline is quit, end the loop
        if (strcmp (cmdline, "quit") == 0) {
            break;
        }

        // get game_id and field from the cmdline
        // char* game_id = strtok (cmdline, " ");
        // char* field;
        // if (game_id != NULL) {
        //     field = strtok (NULL, " \n");
        // }

        // parse the first token, if token isn't NULL then proceed
        char* token = strtok(input, " \n");
        if (token != NULL) {
            //set the first byte to be the size of the cmdline
            input[0] = (unsigned) strlen(cmdline);
            input[1] = '\0';

            //concat the message into the payload
            strcat(input, cmdline);

            //send the message
            write(clientfd, input, sizeof(input));

            // read the return message
            read(clientfd, output, sizeof(output));

            // set it to print form
            int i = 0;
            for (;i < (int) output[0]; i++){ 
                response[i] = output[i+1];    
            }
            response[i] = '\0';

            //print response
            printf("%s\n", response);
        }
    }
    close(clientfd);
    return 0;
}