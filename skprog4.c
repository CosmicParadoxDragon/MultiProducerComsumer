/**
 * @author Sohrab Kazak
 * @version 0.1
 * @brief The fourth project of the CS 421 course
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define BUFFER_SIZE 5 

// typedef struct {
//     int value;
//     struct process *list;
// } semaphore;

// void wait(semaphore *S)
// {
//     S->value--;
//     if (S->value < 0)
//     {

//     }
// }

void *producer(void *param);
void *consumer(void *param);
int insert_item(int item);
int remove_item();
void print_buffer();

int *buffer;
int in;
int out;
int full;
int empty;
sem_t full_sema;
sem_t empty_sema;
sem_t mutex_sema;
int ret;

int main(int argc, char *argv[])
{
    /* 1. Get command line arguments */
    int time = atoi(argv[1]);
    int num_producer_threads = atoi(argv[2]);
    int num_consumer_threads = atoi(argv[3]);
    
    /* 2. Initialize buffer */
    buffer = malloc(BUFFER_SIZE * sizeof(int));
    in = 0;
    out = 0;
    full = 0;
    empty = BUFFER_SIZE;
    ret = sem_init(&full_sema, 0, full);
    ret = sem_init(&empty_sema, 0, empty);
    ret = sem_init(&mutex_sema, 0, 1);

    /* 3. Create Producer Threads */
    pthread_t prod_tid [num_producer_threads];
    int prod_threads_localtid[num_producer_threads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int a;
    for (a = 0; a < num_producer_threads; a++)
    {
        prod_threads_localtid[a] = a + 1;
        pthread_create(&prod_tid[a], &attr, producer, (void* ) &prod_threads_localtid[a]);
        printf("Creating Producer Thread With ID %d\n", prod_threads_localtid[a]);
    }

    /* 4. Create Consumer Threads */
    pthread_t cons_tid [num_consumer_threads];
    int cons_threads_localtid[num_consumer_threads];
    for(a = 0; a < num_consumer_threads; a++)
    {
        cons_threads_localtid[a] = a + 1;
        pthread_create(&cons_tid[a], &attr, consumer, (void* ) &cons_threads_localtid[a]);
        printf("Creating Consumer Thread with ID %d\n", prod_threads_localtid[a]);
    }

    /* 5. Sleep */
    printf("Main Thread Sleeping for %d Seconds\n", time);
    sleep(time);
    // int c;
    // for (c = 0; c < num_producer_threads; c ++)
    // {
    //     pthread_join(prod_tid[c], NULL);
    // }
    
    // for (c = 0; c < num_consumer_threads; c ++)
    // {
    //     pthread_join(cons_tid[c], NULL);
    // }
    

    /* 6. Main thread exits */
    printf("Main Thread Exiting\n");
    exit(0);
}

void *producer(void *param)
{
    int id = *((int *) param);
    int item;

    while(1)
    {
        /* Generate Random Number */
        int sleep_time = (rand() % 4) + 1;
        printf("Producer thread %d sleeping for %d seconds\n", id, sleep_time);
        sleep(sleep_time);
        item = (rand() % 50) + 1;
        printf("producer produced %d\n", item);
        if (insert_item(item) < 0)
        {
            printf("Producer Error\n");
            pthread_exit(0);
        }
        printf("Producer thread %d inserted value %d\n", id, item);
    }
}

void *consumer(void *param)
{
    int id = *((int *) param);
    int item;

    while(1)
    {
        /* Generate Random Number */
        int sleep_time = (rand() % 4) + 1;
        printf("Consumer thread %d sleeping for %d seconds\n", id, sleep_time);
        sleep(sleep_time);
        if (remove_item(&item) < 0)
        {
            printf("Consumer Error\n");
            pthread_exit(0);
        }
        printf("Consumer thread %d removed value %d\n", id, item);
    }
}

int insert_item(int item)
{
    int ret;
    ret = sem_wait(&empty_sema);
    ret = sem_wait(&mutex_sema);
    buffer[in] = item;
    printf("Insert_item inserted item %d at position %d\n", item, in);
    print_buffer();
    in = (in + 1) % BUFFER_SIZE;

    ret = sem_post(&mutex_sema);
    ret = sem_post(&full_sema);
    return ret;
}

int remove_item(int *item)
{
    int ret;
    ret = sem_wait(&full_sema);
    ret = sem_wait(&mutex_sema);
    *item = buffer[out];
    buffer[out] = 0;
    printf("Remove_item removed item %d at position %d\n", *item, out);
    print_buffer();
    out = (out + 1) % BUFFER_SIZE;

    ret = sem_post(&mutex_sema);
    ret = sem_post(&empty_sema);
    return ret;
}

void print_buffer()
{
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        if (buffer[i] <= 0)
        {
            printf("[ empty ]");
        }
        else 
        {
            printf("[ %d ]", buffer[i]);
        }
    }
    printf(" in=%d, out=%d\n", in, out);
}