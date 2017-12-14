#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//  Thread function
void* my_thread(void* arg)
{
    int thread_num = *((int*) arg);
    printf("Thead number %d\n", thread_num);
    return 0;
}

int main(int argc, char **argv)
{
    // Number of threads we wish to create
    int size = 20;
    // create array of thread ids
    pthread_t tid[size];

    // create array of readable numbers
    int tNum[size];
    // create 20 threads
    for(int x = 0; x < size; x++)
    {
        // Because the main thread will run independant of the threads you create
        // we can essentially crate a race condition if we simply pass x as an argument
        // as x can be incremented ++ before the thread has had a chance to read the value
        // below we are effectively creating a local store of the values to avoid race contition.
        tNum[x] = x;
        // pthread_create creates the new thread with a reference to the thread id,
        // second arg refers to thread attributes which for the most part can be left unchanged
        // my_thread is a function pointer the thread pointer
        // last arguement is an option argurment that can be passed to the function pointer
        pthread_create(&tid[x], NULL, my_thread, &tNum[x]);
    }

    // pthread_join joins the created threads to the main thread, without this main would
    // simply finish before the created threads had a chance to run
    for(int x = 0; x < size; x++)
    {
        pthread_join(tid[x], NULL);
    }

    return 0;
}
