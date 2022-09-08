#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct Task {
    void (*task)(void* arg1, void* arg2, void* arg3);
    void* taskArg1;
    void* taskArg2;
    void* taskArg3;
} Task;

typedef struct ThreadPoolState {
    int threadPoolSize;
    pthread_t** threads;
    
    int tasksBufferLength;
    Task** tasks;
    
    int taskIndexRead;
    int taskIndexWrite;
    
    long totalTasksProcessed;
    
    pthread_cond_t* conditionRead;
    pthread_cond_t* conditionWrite;
    
    pthread_mutex_t* mutexRead;
    pthread_mutex_t* mutexWrite;

} ThreadPoolState;

ThreadPoolState* threadPool_start (int threadPoolSize, int tasksBufferLength);
            void threadPool_stop (ThreadPoolState* threadPoolState);
            void threadPool_pushTask (ThreadPoolState* threadPoolState, Task* task);

#endif
