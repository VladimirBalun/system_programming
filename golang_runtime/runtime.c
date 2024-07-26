#include "runtime.h"

#include "utils.h"
#include "scheduler.h"
#include "thread_pool.h"
#include "coroutines_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MIN_THREADS_NUMBER     1
#define MAX_THREADS_NUMBER     64
#define DEFAULT_THREADS_NUMBER 4

typedef struct runtime_t {
    thread_pool_t*      thread_pool;
    coroutines_queue_t* global_queue;
} runtime_t;

static void* __thread_local_schedule(void* thread_parameter)
{
    if (!thread_parameter) {
        fprintf(stderr, "[%s:%d] incorrect thread_parameter\n", __FILE__, __LINE__);
        pthread_exit(0);
    }

    scheduler_t* scheduler = (scheduler_t*)(thread_parameter);
    scheduler_run(scheduler);
    scheduler_destroy(scheduler);
    pthread_exit(0);
}

runtime_t* runtime_init(int threads_number)
{
    if (threads_number < MIN_THREADS_NUMBER || threads_number > MAX_THREADS_NUMBER)
        threads_number = DEFAULT_THREADS_NUMBER;

    coroutines_queue_t* queue = coroutines_queue_init(threads_number);
    if (!queue) {
        fprintf(stderr, "[%s:%d] failed to create coroutines queue\n", __FILE__, __LINE__);
        return NULL;
    }

    thread_pool_t* thread_pool = thread_pool_init(threads_number);
    if (!thread_pool) {
        coroutines_queue_destroy(&queue);
        fprintf(stderr, "[%s:%d] failed to create thread_pool\n", __FILE__, __LINE__);
        return NULL;
    }

    runtime_t* runtime = malloc(sizeof(runtime_t));
    if (!runtime) {
        coroutines_queue_destroy(&queue);
        thread_pool_destroy(&thread_pool);
        fprintf(stderr, "[%s:%d] failed to allocate memory for runtime\n", __FILE__, __LINE__);
        return NULL;
    }

    runtime->thread_pool = thread_pool;
    runtime->global_queue = queue;
    return runtime;
}

void runtime_run(runtime_t* runtime, coroutine_fn_t main_coroutine_fn)
{
    if (!runtime) {
        fprintf(stderr, "[%s:%d] incorrect runtime\n", __FILE__, __LINE__);
        return;
    }

    if (!main_coroutine_fn) {
        fprintf(stderr, "[%s:%d] incorrect main_coroutine_fn\n", __FILE__, __LINE__);
        return;        
    }

    int threads_number = thread_pool_threads_number(runtime->thread_pool);
    for (int thread_idx = 0; thread_idx < threads_number; ++thread_idx) {
        scheduler_t* scheduler = scheduler_init(runtime->global_queue);
        if (!scheduler) {
            fprintf(stderr, "[%s:%d] failed to create scheduler\n", __FILE__, __LINE__);
            continue;
        }

        if (thread_idx == threads_number - 1)
            scheduler_spawn_coroutine(scheduler, main_coroutine_fn);
        thread_pool_run_thread(runtime->thread_pool, __thread_local_schedule, scheduler);
    }

    thread_pool_wait_threads(runtime->thread_pool);
}

void runtime_destroy(runtime_t** runtime)
{
    if (!runtime || !(*runtime)) {
        fprintf(stderr, "[%s:%d] incorrect runtime\n", __FILE__, __LINE__);
        return;
    }

    thread_pool_destroy(&(*runtime)->thread_pool);
    coroutines_queue_destroy(&(*runtime)->global_queue);
    SAFE_DELETE(*runtime);
}