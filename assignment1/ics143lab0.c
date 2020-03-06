#include <stdio.h>
#include <string.h>

// /* read a line from user */
// void readline(char str[]) {
//   int i = 0;
//   char ch;
//   while((ch = getchar()) != '\n' && ch != EOF ) {
//     str[i++] = ch;
//   }
//   str[i] = '\0';
// }

int main() {
    char cmdline[256];
    
    // if the cmdline is quit, then end the loop
    while (1) {
        // print a prompt
        printf("ICS143A>");
        char c = getchar();
        printf("Char Read: %c\n", c);

        // read command line
        int i = 0;
        char ch;
        while((ch = getchar()) != '\n') {
            cmdline[i++] = ch;
        }
        cmdline[i] = '\0';
        
        // evaluate the cmdline
        if (strcmp(cmdline, "exit") == 0) {
            break;
        } else if (strcmp(cmdline, "whoami") == 0) {
            printf("Xuan Liu\n");
        } else {
            printf("invalid command\n");
        }
    }
    return 0;
}

