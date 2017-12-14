/* Interview prep Semaphores
 * jamesmcskeane
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <stdlib.h>

// Producer consumer problem solution
// We create 3 semaphores
sem_t capacity;
sem_t has_data;
sem_t mutex;

// We create a shared resource in the form of a queue
std::queue<int> my_queue;

// Producer thread function
void* producer(void* args)
{
    int num;
    int tNum = *((int*) args);
    while (true)
    {
        // Generate some random data to push to shared resource
        num = (rand()%1000)+1;
        // The first semaphore is the capacity semaphore, this semaphore controls
        // how much data we can push onto the shared resource before it blocks in
        // this example I have set this to 100
        sem_wait(&capacity);
        // Before we can push data to the sharaed resource we need to aquire a lock
        // the push function is essentiall what is known as the critical section of code.
        // The lock will stop race conditions.
        // 1. Two producers trying to write to the same memory block in the queue
        // 2. A producer and consumer pushing and popping data from the queue at the same time
        // The lock is also placed inside the decrementing of the Capacity Semaphore and the
        // incrementing of the has_data semaphore. Doing outside of these two semaphores could
        // result in a deadlock scenario i.e. A Producer locked waiting on a Consumer and a Consumer
        // blocked waiting on a producer.
        sem_wait(&mutex); //Aquire lock
        printf("Pushing %d onto thread_%d\n", num, tNum);
        my_queue.push (num); // Critical section
        sem_post(&mutex); //Release lock
        // This semaphore signals the consumer to ensures there is data in the queue before
        // any consumer attempts to pop data from the queue
        sem_post(&has_data);
    }
    return 0;
}

void* consumer(void* args)
{
    int tNum = *((int*) args);
    while (true)
    {
        // The has_data semaphore is defaultd to 0 therefore will block until the producer
        // pushes data onto it singnaling that there is data in the queue to be consumed
        sem_wait(&has_data);
        // Once we know there is data in the queue we need to aquire the lock again to ensure
        // no race conditions, again the lock is inside the had_data semaphore to also ensure
        // no deal lock scenarios
        sem_wait(&mutex); //Aquire lock
        printf("Popping %d off thread_%d\n", my_queue.front(), tNum);
        my_queue.pop (); // Critical section
        sem_post(&mutex); //Release lock
        // Once the data has been consumed we can also increment the capacity semaphore signaling
        // that the producer can push more data to our shared resource..
        sem_post(&capacity);
    }
    return 0;
}

int main(int arcc, char **argv)
{
    // Initializing the semaphore takes 3 parameters.
    // Parameter 1 is the semaphore
    // Parameter 2 indicates whether the semaphore is local to this single process
    // Parameter 3 indicates the starting value of S in the semaphore, remember if
    // S becomes negative then the semaphore will block and be pushed onto Q
    sem_init(&capacity, 0, 100); // Producer could add 100 elements to shared resourse
    sem_init(&has_data, 0, 0); // 0 will mean the consumer will wait before it attempts to pop
    sem_init(&mutex, 0, 1); // This semaphore is used purely as lock to enure mutual exclsivty

    // Create a few threads
    pthread_t p_thread_1;
    pthread_t c_thread_1;
    pthread_t p_thread_2;
    pthread_t c_thread_2;

    // Create a few args for threads
    int t1 = 1;
    int t2 = 2;

    // Create threads
    pthread_create(&p_thread_1, NULL, producer, &t1);
    pthread_create(&c_thread_1, NULL, consumer, &t1);
    pthread_create(&p_thread_2, NULL, producer, &t2);
    pthread_create(&c_thread_2, NULL, consumer, &t2);

    // Join threads to main thread
    pthread_join(p_thread_1, NULL);
    pthread_join(c_thread_1, NULL);
    pthread_join(p_thread_2, NULL);
    pthread_join(c_thread_2, NULL);

    return 0;
}


