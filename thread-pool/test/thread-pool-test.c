#include <unistd.h>

#include "../src/thread-pool.h"

void testTask(void* arg1, void* arg2, void* arg3) {
    int* i = (int*)arg1;
    printf("Test task %d\n", *i);
    usleep(50000);
}

void testThreadPool() {
    ThreadPoolState* state = threadPool_start(4, 10);

    for (int i = 0; i < 100; i++) {
        int* ii = (int*) malloc(sizeof(int));
        *ii = i;
        Task* task = malloc(sizeof(Task));
        task->task = &testTask;
        task->taskArg1 = ii;

        threadPool_pushTask(state, task);
    }

    threadPool_stop(state);
}

int main (void* args) {
    testThreadPool();
}