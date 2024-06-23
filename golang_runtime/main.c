#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "coroutine.h"
#include "scheduler.h"
#include "coroutines_queue.h"

#define GO_RUNTIME(function) \
    coroutines_queue_t* queue = coroutines_queue_init(); \
    scheduler_t* scheduler = scheduler_init(queue); \
    scheduler_spawn_coroutine(scheduler, (function)); \
    scheduler_run(scheduler); \
    scheduler_destroy(scheduler)

#define GO(function) \
    scheduler_spawn_coroutine(scheduler, (function))

#define PRINT(text) \
    printf("[%ld] - %s", syscall(__NR_gettid), (text)); \
    scheduler_park_coroutine(scheduler)

void coroutine2(scheduler_t* scheduler)
{
    PRINT("coroutine2 started\n");
    PRINT("coroutine2 finished\n");
}

void coroutine1(scheduler_t* scheduler)
{
    PRINT("coroutine1 started\n");
    PRINT("coroutine1 finished\n");
}

void main_coroutine(scheduler_t* scheduler)
{
    PRINT("main coroutine started\n");
    GO(coroutine1);
    PRINT("main middleware\n");
    GO(coroutine2);
    PRINT("main coroutine finished\n");
}

[C1, C2]

Core -> S -> M -> M -> 

int main()
{  
    GO_RUNTIME(main_coroutine);
    return EXIT_SUCCESS;
}