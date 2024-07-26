#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "runtime.h"
#include "coroutine.h"
#include "scheduler.h"
#include "coroutines_queue.h"
#include "thread_pool.h"

#define GO_RUNTIME(function) \
    printf("[%ld] - runtime started\n", syscall(__NR_gettid)); \
    runtime_t* runtime = runtime_init(4); \
    runtime_run(runtime, (function)); \
    runtime_destroy(&runtime); \
    printf("[%ld] - runtime finished\n", syscall(__NR_gettid)); 

#define GO(function) \
    scheduler_spawn_coroutine((coroutine_get_scheduler(coroutine)), (function))

#define PRINT(text) \
    printf("[%ld] - %s", syscall(__NR_gettid), (text)); \
    scheduler_park_coroutine((coroutine_get_scheduler(coroutine)))

void coroutine5(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    PRINT("coroutine5 started\n");
    PRINT("coroutine5 middleware\n");
    PRINT("coroutine5 finished\n");
}

void coroutine4(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    PRINT("coroutine4 started\n");
    PRINT("coroutine4 middleware\n");
    PRINT("coroutine4 finished\n");  
}

void coroutine3(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    PRINT("coroutine3 started\n");
    PRINT("coroutine3 middleware\n");
    PRINT("coroutine3 finished\n");
}

void coroutine2(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    GO(coroutine3);
    GO(coroutine4);
    GO(coroutine5);

    PRINT("coroutine2 started\n");
    PRINT("coroutine2 finished\n");
}

void coroutine1(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    PRINT("coroutine1 started\n");
    PRINT("coroutine1 finished\n");
}

void main_coroutine(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    PRINT("main coroutine started\n");
    GO(coroutine1);
    PRINT("main middleware\n");
    GO(coroutine2);
    PRINT("main coroutine finished\n");
}

int main()
{  
    GO_RUNTIME(main_coroutine);
    return EXIT_SUCCESS;
}