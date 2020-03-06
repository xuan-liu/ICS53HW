#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 80
#define DISKPAGENUM 8
#define MAINMEMPAGENUM 4
#define MAX_ARGS 3
#define AddrPerPage 4
const char* prompt = "> ";
int fifo = 1; // fifo = 1 means we use FIFO as page replacement algorithm
int timeCount = 0; // if there are a read/write operation, the timeCount will increase 1

// define the disk and main memory
struct Page
{
    int timeStamp;
    int addr[AddrPerPage];
    int data[AddrPerPage];
};

struct Page diskMem[DISKPAGENUM];
struct Page mainMem[MAINMEMPAGENUM];

// define the page table
struct PTE
{
    int validBit;
    int dirtyBit;
    int pageNum;
};

struct PTE pageTable[DISKPAGENUM];

// declare functions
void eval (char* cmdline);
void parseline(char *cmdline, char* argv[MAX_ARGS]);
void showptable();
void showmain(int page);
void showdisk(int page);
void initMem ();
void initPtable ();
void readAddr(int addr);
int findAvailablePage();
void writeAddr(int addr, int num);
int findVictimPage();
void showAllmain();

int main(int argc, char *argv[]) {
    // initial the disk and main memory
    initMem();
    initPtable();

    // use FIFO​ as default, if the argument is ​LRU​, then use LRU
    if (argc > 1 && strcmp (argv[1], "LRU") == 0) {
        fifo = 0;
    }

    // check cmdline
    char cmdline[MAXLINE];
    while (1) {
        // print a prompt
        printf("%s", prompt);

        // read cmdline
        fgets(cmdline, MAXLINE, stdin);
        cmdline[strcspn(cmdline, "\n")] = 0;
        
        if (strcmp (cmdline, "quit") == 0) {
            break;
        }
        eval(cmdline);
    }

    return 0;
}

/* initial the main memory and disk memory */
void initMem (){
    // initial the addr and data in the main memory
    int i;
    int count = 0;
    // initial a page
    for (i = 0; i < MAINMEMPAGENUM; i++) {
        mainMem[i].timeStamp = 0;
        int j;
        for (j = 0; j < AddrPerPage; j++) {
            mainMem[i].addr[j] = count;
            mainMem[i].data[j] = -1;
            count += 1;
            //printf("%d %d\n", tmp.addr[j], tmp.data[j]);
        }
    }

    // initial the addr and data in the disk memory
    count = 0;
    // initial a page
    for (i = 0; i < DISKPAGENUM; i++) {
        diskMem[i].timeStamp = 0;
        int j;
        for (j = 0; j < AddrPerPage; j++) {
            diskMem[i].addr[j] = count;
            diskMem[i].data[j] = -1;
            count += 1;
        }
    }
}

/* initial the page table */
void initPtable (){
    int i;
    for (i = 0; i < DISKPAGENUM; i++) {
        pageTable[i].validBit = 0;
        pageTable[i].dirtyBit = 0;
        pageTable[i].pageNum = i;
    }
}

/* evaluate the cmd line, and do according jobs for the cmd line */
void eval (char* cmdline) {
    char* argArray [MAX_ARGS];
    parseline (cmdline, argArray);

    // check the argv[0]
    if (strcmp (argArray[0], "read") == 0) {
        readAddr(atoi(argArray[1]));
    } else if (strcmp (argArray[0], "write") == 0) {
        writeAddr(atoi(argArray[1]), atoi(argArray[2]));
    } else if (strcmp (argArray[0], "showmain") == 0) {
        showmain(atoi(argArray[1]));
    } else if (strcmp (argArray[0], "showdisk") == 0) {
        showdisk(atoi(argArray[1]));
    } else if (strcmp (argArray[0], "showptable") == 0) {
        showptable();
    } 
    // else if (strcmp (argArray[0], "showAllmain") == 0) {
    //     showAllmain();
    // }
}

/* parse the cmdline into array */
void parseline(char *cmdline, char* argv[MAX_ARGS]){
    // parse the cmd according to space
    char* pch = strtok (cmdline," ");
    int idx = 0;

    while (pch != NULL)
    {
        argv[idx] = pch;
        pch = strtok (NULL, " ");
        idx += 1;
    }
}

/* print the page table */
void showptable(){
    int i;
    for (i = 0; i < DISKPAGENUM; i++) {
        struct PTE tmp = pageTable[i];
        printf("%d:%d:%d:%d\n", i, tmp.validBit, tmp.dirtyBit, tmp.pageNum);
    }
}

/* print the timeStamp of the main memory, use for debug */
// void showAllmain(){
//     int i;
//     for (i = 0; i < 4; i++){
//         printf("%d:%d\n", mainMem[i].addr[i] >> 2, mainMem[i].timeStamp);
//     }
// }

/* print the contents of a physical page in main memory */
void showmain(int page){
    int i;
    for (i = 0; i < AddrPerPage; i++){
        printf("%d:%d\n", mainMem[page].addr[i], mainMem[page].data[i]);
    }
}

/* print the contents of a physical page in disk */
void showdisk(int page){
    int i;
    for (i = 0; i < AddrPerPage; i++){
        printf("%d:%d\n", diskMem[page].addr[i], diskMem[page].data[i]);
    }
}

/* print the contents of a virtual memory address */
void readAddr(int addr) {
    int virtualID = addr >> 2;
    // if page hit, then get the main memory address and print the data
    if (pageTable[virtualID].validBit) {
        // printf("A Page Hit Has Occurred\n");
        printf("%d\n", mainMem[pageTable[virtualID].pageNum].data[addr % AddrPerPage]);

        // if LRU, update the timeStamp of that page
        timeCount += 1;
        if (!fifo) {
            mainMem[pageTable[virtualID].pageNum].timeStamp = timeCount;
        }
    } else {
        // if page fault, then find first available page in page table
        printf("A Page Fault Has Occurred\n");
        int availablePage = findAvailablePage();
        if (availablePage == -1) {
            // If there is no available page, find a victim page in main memory
            int victim = findVictimPage();
            // printf("no available page, victim page is %d\n", victim);

            // Copy victim to disk if it is dirty
            int vicDiskPage = mainMem[victim].addr[0] >> 2;
            diskMem[vicDiskPage] = mainMem[victim];

            // Copy disk page to victim page
            mainMem[victim] = diskMem[pageTable[virtualID].pageNum];
            printf("%d\n", mainMem[victim].data[addr % AddrPerPage]);

            // update the timeStamp of that page
            timeCount += 1;
            mainMem[victim].timeStamp = timeCount;

            // Update page table
            pageTable[virtualID].validBit = 1;
            pageTable[virtualID].pageNum = victim;

            pageTable[vicDiskPage].validBit = 0;
            pageTable[vicDiskPage].dirtyBit = 0;
            pageTable[vicDiskPage].pageNum = vicDiskPage;
        } else {
            // If there is an available page, Copy disk page to available main memory page
            // printf("available page is %d\n", availablePage);
            mainMem[availablePage] = diskMem[pageTable[virtualID].pageNum];
            printf("%d\n", mainMem[availablePage].data[addr % AddrPerPage]);

            // update the timeStamp of that page
            timeCount += 1;
            mainMem[availablePage].timeStamp = timeCount;

            // Update page table
            pageTable[virtualID].validBit = 1;
            pageTable[virtualID].pageNum = availablePage;
        }
    }
}

/* write number to a virtual memory address */
void writeAddr(int addr, int num){
    int virtualID = addr >> 2;
    // update dirtyBit
    pageTable[virtualID].dirtyBit = 1;

    // if page hit, then get the main memory address and write the data
    if (pageTable[virtualID].validBit) {
        // printf("A Page Hit Has Occurred\n");
        mainMem[pageTable[virtualID].pageNum].data[addr % AddrPerPage] = num;

        // if LRU, update the timeStamp of that page
        timeCount += 1;
        if (!fifo) {
            mainMem[pageTable[virtualID].pageNum].timeStamp = timeCount;
        }

    } else {
        // if page fault, then find first available page in page table
        printf("A Page Fault Has Occurred\n");
        int availablePage = findAvailablePage();
        if (availablePage == -1) {
            // If there is no available page, find a victim page in main memory
            int victim = findVictimPage();
            // printf("no available page, victim page is %d\n", victim);

            // Copy victim to disk if it is dirty
            int vicDiskPage = mainMem[victim].addr[0] >> 2;
            diskMem[vicDiskPage] = mainMem[victim];

            // Copy disk page to victim page
            mainMem[victim] = diskMem[pageTable[virtualID].pageNum];
            mainMem[victim].data[addr % AddrPerPage] = num;

            // update the timeStamp of that page
            timeCount += 1;
            mainMem[victim].timeStamp = timeCount;

            // Update page table
            pageTable[virtualID].validBit = 1;
            pageTable[virtualID].pageNum = victim;

            pageTable[vicDiskPage].validBit = 0;
            pageTable[vicDiskPage].dirtyBit = 0;
            pageTable[vicDiskPage].pageNum = vicDiskPage;
        } else {
            // If there is an available page, Copy disk page to available main memory page
            // printf("available page is %d\n", availablePage);
            mainMem[availablePage] = diskMem[pageTable[virtualID].pageNum];
            mainMem[availablePage].data[addr % AddrPerPage] = num;

            // update the timeStamp of that page
            timeCount += 1;
            mainMem[availablePage].timeStamp = timeCount;

            // Update page table
            pageTable[virtualID].validBit = 1;
            pageTable[virtualID].pageNum = availablePage;
        }
    }
}

/* find the first available page in main memory, if we find one, return the virtual page number,
if we cannot find one, return -1 */
int findAvailablePage(){
    int pageValid[MAINMEMPAGENUM]; // record whether this main memory page is in virtual
    int i;
    for (i = 0; i < MAINMEMPAGENUM; i++) {
        pageValid[i] = 0;
    }
    // fill in pageValid
    for (i = 0; i < DISKPAGENUM; i++) {
        if (pageTable[i].validBit == 1) {
            pageValid[pageTable[i].pageNum] = 1;
        }
    }
    // check pageValid to get the first available page
    for (i = 0; i < MAINMEMPAGENUM; i++) {
        if (pageValid[i] == 0) {
            return i;
        }
    }
    return -1;
}

/* find the victim page in main memory */
int findVictimPage(){
    // iterate all main memory page, find the smallest timeStamp
    int i;
    int minTime = mainMem[0].timeStamp;
    int minPage = 0;
    for (i = 1; i < MAINMEMPAGENUM; i++) {
        if (mainMem[i].timeStamp < minTime) {
            minTime = mainMem[i].timeStamp;
            minPage = i;
        }
    }
    return minPage;
}