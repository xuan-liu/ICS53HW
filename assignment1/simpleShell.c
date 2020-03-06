#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SHELL_LINES 25
#define MAXLINE 80
#define MAXARGS 20

// define global constant
const char* prompt = "prompt>";
const char background_identifier = '&';

// declare functions
int parseline(char *cmdline, char* argv[MAXARGS], char** inputFile, char** outputFile);
void eval(char *cmdline);
void child_handler(int sig);
void trim(char * str);

int main() {
    char cmdline[MAXLINE];
    
    // if the cmdline is quit, then end the loop
    while (1) {
        // print a prompt
        printf("%s", prompt);

        // get the cmdline and delete the \n at last of the line
        fgets(cmdline, MAXLINE, stdin);
        cmdline[strcspn(cmdline, "\n")] = 0;

        if (strcmp(cmdline, "quit") == 0) {
            break;
        }
        eval(cmdline);
    }
    return 0;
}

/* evaluate the cmd line, if not builtin command, fork child process to do the job */
void eval(char *cmdline){
    // parse the cmdline into array
    char *argv[MAXARGS]; // argv for execv()
    int bg;
	pid_t pid; // process id
    char* inputFile; // redirect input filename
    char* outputFile; // redirect output filename

	bg = parseline (cmdline, argv, &inputFile, &outputFile); 

    // create child to run user job
    if (strcmp(cmdline, "quit") != 0) {
        if ((pid = fork()) == 0) {
            // redirect to file
            if (inputFile != NULL){
                if (freopen(inputFile, "r", stdin) == NULL) {
                    printf("Read Error: Cannot read from file %s.\n", inputFile);
                }
            }

            if (outputFile != NULL){
                if (freopen(outputFile, "w", stdout) == NULL) {
                    printf("Write Error: Cannot write to file %s.\n", outputFile);
                }
            }
            
            // execute the program
            if (execv(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }

        }
        // if fg, parent waits for fg job to terminate
        if (!bg) {
            int status;
            waitpid(pid, &status, 0);
            // if (waitpid(pid, &status, 0) < 0) {
            //     printf("waitfg: waitpid error\n");
            // }
        }
        // if bg, don't wait the child but handle the signal when the child exit
        else {
            //printf("%d %s", pid, cmdline);
            signal(SIGCHLD, child_handler);
        }
    }


}

/* define a handler for SIGCHLD, if child end, the parent reap it */
void child_handler(int sig){
    pid_t wpid = wait(NULL);
    // printf("%d is end\n", wpid);
}

/* set the redirect i/o file, parse the cmdline into array, check whether the cmdline end with &. 
if yes, return 1 and delete the & in the line. if no, return 0*/
int parseline(char *cmdline, char* argv[MAXARGS], char** inputFile, char** outputFile){
    // check whether the cmdline end with &
    int bg;
    int len = strlen(cmdline);
    
    if (cmdline[len - 1] == background_identifier) {
        bg = 1;
        cmdline[len - 1] = 0;
    } else {
        bg = 0;
    }

    // set the redirect i/o file
    *inputFile = NULL;
    *outputFile = NULL;

    // check whether cmdline has "<", if yes, set inputFile
    char* cmd = strtok (cmdline,"<");
    if (cmd != NULL){
        char* tmp = strtok (NULL, "<");

        if (tmp != NULL) {
            // if cmdline has "<", check whether tmp has ">", if yes, set outputFile
            *inputFile = strtok (tmp, ">");

            if (*inputFile != NULL) {
                *outputFile = strtok (NULL, ">");
            }
        } 
        // check whether cmdline has ">", if yes, set outputFile
        cmd = strtok (cmdline,">");
        if (cmd != NULL && *outputFile == NULL) {
            *outputFile = strtok (NULL, ">");
        }
    }

    // Trim leading and trailing white spaces in *inputFile and *outputFile
    if (*inputFile != NULL) trim(*inputFile);
    if (*outputFile != NULL) trim(*outputFile);

    // parse the cmd according to space
    char* pch = strtok (cmd," ");
    int idx = 0;

    while (pch != NULL)
    {
        argv[idx] = pch;
        pch = strtok (NULL, " ");
        idx += 1;
    }
    return bg;
}


/* Remove leading and trailing white space characters */
void trim(char * str)
{
    int index, i;
    // Trim leading white spaces
    index = 0;
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    // Shift all trailing characters to its left 
    i = 0;
    while(str[i + index] != '\0')
    {
        str[i] = str[i + index];
        i++;
    }
    str[i] = '\0'; // Terminate string with NULL


    // Trim trailing white spaces
    i = 0;
    index = -1;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index = i;
        }
        i++;
    }

    // Mark the next character to last non white space character as NULL
    str[index + 1] = '\0';
}