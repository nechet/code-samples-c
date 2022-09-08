#include "thread-pool.h"

typedef struct TaskThreadState {
    int* index;
    ThreadPoolState* state;
} TaskThreadState;

void* threadPool_taskExecutor (void* arg) {
    TaskThreadState* threadState = (TaskThreadState*) arg;
    ThreadPoolState* state = threadState->state;

    printf("Thread Executor %d is ready\n", *threadState->index);

    Task* task;

    while(1) {
        pthread_mutex_lock (state->mutexRead);

        while (state->taskIndexRead == state->taskIndexWrite) {
            pthread_cond_wait (state->conditionRead, state->mutexRead);
        }
        int i = state->taskIndexRead;
        task = state->tasks[i];
        i++;
        i = i >= state->tasksBufferLength ? 0 : i;
        state->taskIndexRead = i;
        state->totalTasksProcessed++;

        pthread_mutex_unlock (state->mutexRead);

        task->task (task->taskArg1, task->taskArg2, task->taskArg3);

        if (state->taskIndexRead - state->taskIndexWrite == 1
            || !(state->taskIndexRead == 0 && state->taskIndexWrite == state->tasksBufferLength - 1)) {
            pthread_mutex_lock (state->mutexWrite);
            pthread_cond_signal (state->conditionWrite);
            pthread_mutex_unlock (state->mutexWrite);
        }

        printf ("Task, write index %d, read index: %d popped, total: %ld\n",
            state->taskIndexWrite, state->taskIndexRead, state->totalTasksProcessed);

        free(task->taskArg1);
        free(task->taskArg2);
        free(task->taskArg3);
        free(task);
    }
    printf("Thread Executor %d stopped\n", *threadState->index);

    free(threadState->index);
    free(threadState);
}


ThreadPoolState* threadPool_start (int threadPoolSize, int tasksBufferLength) {
    ThreadPoolState* state = (ThreadPoolState*) malloc(sizeof(ThreadPoolState));
    state->totalTasksProcessed = 0;
    state->taskIndexRead = 0;
    state->taskIndexWrite = 0;
    state->tasksBufferLength = tasksBufferLength;
    state->tasks = (Task**) malloc(tasksBufferLength * sizeof(Task*));
    state->threadPoolSize = threadPoolSize;
    state->threads = (pthread_t**) malloc(threadPoolSize * sizeof(pthread_t*));
    state->conditionRead = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    state->conditionWrite = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    state->mutexRead = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    state->mutexWrite = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));

    if (pthread_cond_init(state->conditionRead, NULL) != 0) {
        perror("Failed to init condition\n");
    }
    if (pthread_cond_init(state->conditionWrite, NULL) != 0) {
        perror("Failed to init condition\n");
    }
    if (pthread_mutex_init(state->mutexRead, NULL) != 0) {
        perror("Failed to init mutexRead\n");
    }
    if (pthread_mutex_init(state->mutexWrite, NULL) != 0) {
        perror("Failed to init mutexWrite\n");
    }

    for (int i = 0; i < threadPoolSize; i++) {
        TaskThreadState* threadState = (TaskThreadState*) malloc(sizeof(TaskThreadState));
        threadState->state = state;
        threadState->index = (int*) malloc(sizeof(int));
        *threadState->index = i;

        state->threads[i] = (pthread_t*) malloc(sizeof(pthread_t));
        if (pthread_create(state->threads[i], NULL, &threadPool_taskExecutor, threadState) != 0) {
            perror("Failed to create thread\n");
        }
    }

    printf("Thread pool started...\n");

    return state;
}


void threadPool_stop (ThreadPoolState* state) {
    for (int i = 0; i < state->threadPoolSize; i++) {
        pthread_cancel(*state->threads[i]);
    }
    // for (int i = 0; i < state->threadPoolSize; i++) {
    //     pthread_join(state->threads[i], NULL);
    // }
    pthread_cond_destroy(state->conditionRead);
    pthread_cond_destroy(state->conditionWrite);
    pthread_mutex_destroy(state->mutexRead);
    pthread_mutex_destroy(state->mutexWrite);

    free(state->tasks);
    free(state->threads);
    free(state->conditionRead);
    free(state->mutexRead);
    free(state->mutexWrite);
    free(state);

    printf("Thread pool stopped...\n");
}


void threadPool_pushTask (ThreadPoolState* state, Task* task) {
    pthread_mutex_lock (state->mutexWrite);

    while (state->taskIndexRead - state->taskIndexWrite == 1
            || (state->taskIndexRead == 0 && state->taskIndexWrite == state->tasksBufferLength - 1)) {
        pthread_cond_wait (state->conditionWrite, state->mutexWrite);
    }

    int i = state->taskIndexWrite;
    state->tasks[i] = task;
    i = ++i >= state->tasksBufferLength ? 0 : i;
    state->taskIndexWrite = i;

    pthread_mutex_unlock (state->mutexWrite);

    if (state->taskIndexRead != state->taskIndexWrite) {
        pthread_mutex_lock (state->mutexRead);
        pthread_cond_signal (state->conditionRead);
        pthread_mutex_unlock (state->mutexRead);
    }

    printf ("Task, write index %d, read index: %d pushed\n", i, state->taskIndexRead);
}
