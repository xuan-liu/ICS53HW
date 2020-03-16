// name: Xuan Liu, student ID: 35689116
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UINT16_MAX 65535
#define MAX_CMD_LINE 80
#define HEAP_SIZE 127
#define MAX_ARGS 3
const char* prompt = "> ";

unsigned char* heap;

// declare functions
void eval (char* cmdline);
void parseline(char *cmdline, char* argv[MAX_ARGS]);
void initHeap(int numBytes);
void pack(unsigned char* heap, int addr, unsigned char block_size, unsigned char alloc_flag);
void blocklist();
void myMalloc(unsigned int numBytes);
unsigned int allocateBlock(unsigned int numBytes);
void splitBlock(unsigned int addr, unsigned int numBytes);
void myFree(unsigned int addr);
void writemem(unsigned int addr, char* str);
void printmem(unsigned int addr, int size);

int main(int argc, char *argv[]) {
    // initial the heap
    initHeap(HEAP_SIZE);

    // check cmdline
    char cmdline[MAX_CMD_LINE];
    while (1) {
        // print a prompt
        printf("%s", prompt);

        // read cmdline
        fgets(cmdline, MAX_CMD_LINE, stdin);
        cmdline[strcspn(cmdline, "\n")] = 0;
        
        if (strcmp (cmdline, "quit") == 0) {
            break;
        }
        eval(cmdline);
    }

    return 0;
}

/* evaluate the cmd line, and do according jobs for the cmd line */
void eval (char* cmdline) {
    char* argArray [MAX_ARGS];
    parseline (cmdline, argArray);

    // check the argv[0]
    if (strcmp (argArray[0], "malloc") == 0) {
        char* end; 
        myMalloc(strtoul(argArray[1], &end, 0));
    } else if (strcmp (argArray[0], "free") == 0) {
        char* end; 
        myFree(strtoul(argArray[1], &end, 0));
    } else if (strcmp (argArray[0], "blocklist") == 0) {
        blocklist();
    } else if (strcmp (argArray[0], "writemem") == 0) {
        char* end; 
        writemem(strtoul(argArray[1], &end, 0), argArray[2]);
    } else if (strcmp (argArray[0], "printmem") == 0) {
        // printf("printmem %d %d\n", atoi(argArray[1]), atoi(argArray[2]));
        char* end; 
        printmem(strtoul(argArray[1], &end, 0), atoi(argArray[2]));
    } 
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

/* Initialize Heap as a size of numBytes */
void initHeap(int numBytes) {
	// Create and Allocate Heap
	heap = (unsigned char*) malloc(numBytes * sizeof(unsigned char));

	// Create an initial Block 
	unsigned char block_size = numBytes;
	unsigned char alloc_flag = 0;
	pack(heap, 0, block_size, alloc_flag);

    // initial all memory as 0 except the header
    int i;
    for (i = 1; i < HEAP_SIZE; i++) {
        heap[i] = '0';
    }
}

/* set the header of the block */
void pack(unsigned char* heap, int addr, unsigned char block_size, unsigned char alloc_flag){
    // generate the header from block_size and alloc_flag
    unsigned char tmp = ((block_size << 1) | alloc_flag) & 0xFF;
    heap[addr] = tmp;
    // printf("block_size: %x, alloc_flag: %x, heap: %x\n", block_size, alloc_flag, *heap);
}

/* prints out information about all of the blocks in your heap */
void blocklist(){
    unsigned char* p = heap;
    unsigned int i = 0;
    while ((i < HEAP_SIZE)) {
        unsigned char block_size = (*p) >> 1;
        unsigned char alloc_flag = (*p) & 1;
        if (alloc_flag){
            printf("%d, %d, allocated\n", i + 1, block_size - 1);
        } else
        {
            printf("%d, %d, free\n", i + 1, block_size - 1);
        }
        i += block_size;
        p = p + block_size;
    }
}

/* allocate a block of memory from the heap, the arg is the number of bytes 
which the user wants in the payload of the allocated block */
void myMalloc(unsigned int numBytes){
    // find the best block to malloc
    unsigned int bestAddr = allocateBlock(numBytes);
    printf("%d\n", bestAddr + 1);
    // split the block
    splitBlock(bestAddr, numBytes);
}

/* free a block of memory, the arg is the pointer to the payload of a previously 
allocated block of memory */
void myFree(unsigned int addr){
    // change the alloc_flag in heap[addr] to be 0
    unsigned char* p = heap + addr - 1;
    unsigned char block_size = (*p) >> 1;
    *p = (*p) & 0xFE;

    // coalesce with the next block if the next block is free
    unsigned char* next = p + block_size;
    if ((*next & 0x01) == 0) {
        *p = (block_size + (*next >> 1)) << 1;
    }
}

/* writes alpha-numeric characters into memory */
void writemem(unsigned int addr, char* str){
    unsigned char* p = heap + addr;
    int i = 0;
    while (i < strlen(str)) {
        p[i] = str[i];
        // printf("%c\n", p[i]);
        i += 1;
    }
}

/* prints out a segment of memory in hexadecimal */
void printmem(unsigned int addr, int size){
    unsigned char* p = heap + addr;
    int i = 0;
    while (i < size) {
        printf("%x ", p[i]);
        i += 1;
    }
    printf("\n");
}

/* use the best-fit allocation strategy to search for a block to allocate, return the block's index, if no block can
fit, return -1 */
unsigned int allocateBlock(unsigned int numBytes){
    unsigned char* p = heap;
    unsigned int i = 0;
    unsigned int res = UINT16_MAX; // the result block's index in heap

    while ((i < HEAP_SIZE)) {
        unsigned char block_size = (*p) >> 1;
        unsigned char alloc_flag = (*p) & 1;
        // if the block is free and large enough, we pick the smallest block
        if (!alloc_flag && (block_size - 1) >= numBytes && block_size < res){
            // printf("addr: %d, blockSize: %d\n", i + 1, block_size);
            res = i;
        }
        i += block_size;
        p = p + block_size;
    }
    if (res != UINT16_MAX) {
        return res;
    } else
    {
        return -1;
    }
}

/* if allocated space is smaller than free space, split the block */
void splitBlock(unsigned int addr, unsigned int numBytes){
    unsigned char newsize = numBytes + 1;
    unsigned char* p = heap + addr;
    unsigned char oldsize = (*p) >> 1;
    // set new length
    *p = ((newsize << 1) | 0x01) & 0xFF;
    if (newsize < oldsize) {
        // set length in remaining part of block
        p[newsize] = ((oldsize - newsize) << 1 | 0x00) & 0xFF;
    }
}
