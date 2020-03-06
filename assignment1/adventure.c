#include <stdio.h>
#include <string.h>

#define MAX_LINE_SIZE 120
#define MAX_COMMAND_SIZE 100
#define MAX_NUMBER_ROOM 50
#define MAX_DISCRIPTION_SIZE 100

// define global struct room, current room, and countRoom
struct Room
{
    int roomNum;
    char desc[MAX_DISCRIPTION_SIZE];
    int northRm;
    int southRm;
    int eastRm;
    int westRm;
} rooms[MAX_NUMBER_ROOM];

struct Room curRoom;
int countRoom = 0; // the total number of rooms in the file

// declare functuion
void readDungeonFile(FILE* fptr);
struct Room searchRoom(int n);

void adventure() {
    // Initializing an array of room
    int loadDunOnce = 0;

    // let user command
    char command[MAX_COMMAND_SIZE]; // store the command user put in
    printf("$"); // print prompt
    fgets(command, MAX_COMMAND_SIZE, stdin);

    // if the command is not quit, then run the program
    while (strcmp(command, "quit\n") != 0) {
        // check command is loaddungeon
        if (strlen(command) > 11) {
            char * pch;
            pch = strtok (command, " ");

            if (strcmp(pch, "loaddungeon") == 0 && pch != NULL)
            {
                // check whether the first time to load dungeon
                if (loadDunOnce == 1) {
                    printf("Dungeon already loaded\n");
                    continue;
                } else {
                    loadDunOnce = 1;
                }
            
                pch = strtok (NULL, " \n"); // pch is the file name
                FILE* fptr; 
                fptr = fopen (pch, "r"); // open file

                // read file
                if (fptr == NULL ){
                    printf("Fail To Open File!\n");
                    continue;
                } else {
                    //printf("Successfully open the file\n");
                    readDungeonFile(fptr);
                }
                fclose(fptr);

                // print the current room's description
                printf("%s\n", curRoom.desc);
                
            }
        } else if (strcmp(command, "north\n") == 0) {
            // get the current room's north room number
            int northRm = curRoom.northRm;

            // check the room number 
            if (northRm == -1) {
                printf("You can’t go there.\n");
            } else {
                // change the current room and print the current room's description
                curRoom = searchRoom(northRm);
                printf("%s\n", curRoom.desc);
            }
    
        } else if (strcmp(command, "south\n") == 0) {
            // get the current room's south room number
            int southRm = curRoom.southRm;

            // check the room number 
            if (southRm == -1) {
                printf("You can’t go there.\n");
            } else {
                // change the current room and print the current room's description
                curRoom = searchRoom(southRm);
                printf("%s\n", curRoom.desc);
            }

        } else if (strcmp(command, "east\n") == 0) {
            // get the current room's east room number
            int eastRm = curRoom.eastRm;

            // check the room number 
            if (eastRm == -1) {
                printf("You can’t go there.\n");
            } else {
                // change the current room and print the current room's description
                curRoom = searchRoom(eastRm);
                printf("%s\n", curRoom.desc);
            }

        } else if (strcmp(command, "west\n") == 0) {
            // get the current room's west room number
            int westRm = curRoom.westRm;

            // check the room number 
            if (westRm == -1) {
                printf("You can’t go there.\n");
            } else {
                // change the current room and print the current room's description
                curRoom = searchRoom(westRm);
                printf("%s\n", curRoom.desc);
            }
        } 

        // let user command
        printf("$"); // print prompt
        fgets(command, 100, stdin);
    }
    return;
}

// read the file and set the room array
void readDungeonFile(FILE* fptr){
    char topic_line[MAX_LINE_SIZE];

    while( fgets(topic_line, MAX_LINE_SIZE, fptr) != NULL ) {
        sscanf(topic_line, "%d +%[^+]+ %d %d %d %d", &(rooms[countRoom].roomNum), rooms[countRoom].desc, &(rooms[countRoom].northRm), &(rooms[countRoom].southRm), &(rooms[countRoom].eastRm), &(rooms[countRoom].westRm));

        if (countRoom == 0) {
            //curRoomNum = rooms[countRoom].roomNum;
            curRoom = rooms[countRoom];
        }
        countRoom += 1;
    }
}

// according to the roomNum, find the address of the room
struct Room searchRoom(int n){
    // iterate the rooms array to find the room
    int i;
    for (i = 0; i < countRoom; i += 1) {
        if (rooms[i].roomNum == n) {
            return rooms[i];
        }
    }
    // if cannot search the room, return the last room
    return rooms[countRoom - 1];
}