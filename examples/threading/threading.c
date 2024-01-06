#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)



// Thread function
void* threadfunc(void* thread_param) {
    struct thread_data* thread_func_args = (struct thread_data*) thread_param;

    // TODO: wait, obtain mutex, wait, release mutex
    usleep(thread_func_args->wait_to_obtain_ms * 1000); // Convert milliseconds to microseconds
    pthread_mutex_lock(thread_func_args->mutex);

    // Do work while holding the mutex (protected region)
    DEBUG_LOG("Thread acquired mutex");

    usleep(thread_func_args->wait_to_release_ms * 1000); // Simulate work inside the critical section

    // Release the mutex
    pthread_mutex_unlock(thread_func_args->mutex);
    DEBUG_LOG("Thread released mutex");

    return thread_param;
}

bool start_thread_obtaining_mutex(pthread_t* thread, pthread_mutex_t* mutex, int wait_to_obtain_ms, int wait_to_release_ms) {
    // Allocate memory for thread_data structure
    struct thread_data* thread_data = (struct thread_data*)malloc(sizeof(struct thread_data));

    if (thread_data == NULL) {
        ERROR_LOG("Failed to allocate memory for thread_data");
        return false;
    }

    // Set up thread data
    thread_data->mutex = mutex;
    thread_data->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data->wait_to_release_ms = wait_to_release_ms;

    // Create the thread using threadfunc() as the entry point
    int result = pthread_create(thread, NULL, threadfunc, thread_data);

    if (result != 0) {
        ERROR_LOG("Failed to create thread: %d", result);
        free(thread_data);
        thread_data->thread_complete_success = false;
        return false ;
    }

    thread_data->thread_complete_success = true;
    return true ;
}
