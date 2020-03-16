// name: Xuan Liu, student ID: 35689116
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 20
unsigned int mSeconds = 500000;

// global variables
int buffer[BUFFER_SIZE];
sem_t mutex; // protects accesses to buffer
sem_t slots; // counts available slots
sem_t items; // counts available items
int front, rear; 
// buffer[(front+1)%BUFFER_SIZE] is the first item, buffer[rear%BUFFER_SIZE] is the last item

int num_producer;
int num_consumer;
int num_item_producer;
int num_item_consumer;
int delay_option;

// declare functions
void *producer(void *vargp);
void *consumer(void *vargp);
void insert_item(int item_value, int pid);
int remove_item(int pid);

int main(int argc, char **argv){
    if (argc != 5){
    printf("ERROR: Please provide exactly four arguments.\n");
    exit(1);
    }

    // Retrieve command line arguments
    num_producer = strtol(argv[1], NULL, 0);
    num_consumer = strtol(argv[2], NULL, 0);
    num_item_producer = strtol(argv[3], NULL, 0);
    delay_option = strtol(argv[4], NULL, 0);
    num_item_consumer = (int) (num_producer * num_item_producer)/num_consumer;

    printf("%d %d %d %d %d\n", num_producer, num_consumer, num_item_producer, num_item_consumer, delay_option);

    // Initialize
    sem_init(&mutex, 0, 1);
    sem_init(&slots, 0, BUFFER_SIZE); // initially, buf has n empty slots
    sem_init(&items, 0, 0); // initially, buf has 0 items
    front = rear = 0;

    int a, b;
    sem_getvalue(&slots, &a);
    sem_getvalue(&items, &b);
    printf("slots: %d, items: %d\n", a, b);

    // Create the producer and consumer threads
    int i;
    pthread_t producers[num_producer];
    pthread_t consumers[num_consumer];
    for (i = 0; i < num_producer; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&producers[i], NULL, producer, arg);
    }
    
    for (i = 0; i < num_consumer; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&consumers[i], NULL, consumer, arg);
    }

    // reap the threads
    for(i = 0; i < num_producer; i++){
		pthread_join(producers[i], NULL);
	}
	for(i = 0; i < num_consumer; i++){
		pthread_join(consumers[i], NULL);
	}

    return 0;
}

/* producer thread routine */
void *producer(void *vargp){
    int n = *((int *) vargp);
    free(vargp);

    int i;
    for (i = 0; i < num_item_producer; i++){
        int item_value = n * num_item_producer + i;
        insert_item(item_value, n);

        // add delay
        if (delay_option == 0){
            usleep(mSeconds);
        }
    }
}

/* consumer thread routine */
void *consumer(void *vargp){
    int n = *((int *) vargp);
    free(vargp);

    int i;
    for (i = 0; i < num_item_consumer; i++){
        int value = remove_item(n);

        // add delay
        if (delay_option == 1){
            usleep(mSeconds);
        }
    }
}

/* Insert item into buffer */
void insert_item(int item_value, int pid){
    sem_wait(&slots);
    sem_wait(&mutex);

    // Add item to buffer
    rear += 1;
    buffer[rear % BUFFER_SIZE] = item_value;
    printf("producer_​%d ​produced item %d\n", pid, item_value);

    sem_post(&mutex);
    sem_post(&items);
}

/* remove item into buffer and return the item value*/
int remove_item(int pid){
    sem_wait(&items);
    sem_wait(&mutex);

    // remove the item from buffer
    front += 1;
    int value = buffer[front % BUFFER_SIZE];
    printf("consumer_%d consumed item %d\n", pid, value);

    sem_post(&mutex);
    sem_post(&slots);

    return value;
}
