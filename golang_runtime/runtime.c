#include "runtime.h"

#include "thread_pool.h"

typedef struct runtime_t {
    thread_pool_t*      thread_pool;
    coroutines_queue_t* global_queue;
} runtime_t;

static void* __thread_scheduler(void*)
{
    scheduler_t* scheduler = scheduler_init(queue);
    scheduler_run(scheduler);
    scheduler_destroy(scheduler);  
}

runtime_t* runtime_init(int threads_number)
{
    coroutines_queue_t* queue = coroutines_queue_init();
    if (!queue) {
        fprintf(stderr, "failed to create coroutines queue\n");
        return NULL;
    }

    thread_pool_t* thread_pool = thread_pool_init(threads_number);
    if (!thread_pool) {
        coroutines_queue_destroy(queue);
        fprintf(stderr, "failed to create thread_pool\n");
        return NULL;
    }

    runtime_t* runtime = malloc(sizeof(runtime_t));
    if (!runtime) {
        coroutines_queue_destroy(queue);
        thread_pool_destroy(thread_pool);
        fprintf(stderr, "failed to allocate memory for runtime\n");
        return NULL;
    }

    runtime->thread_pool = thread_pool;
    runtime->global_queue = queue;
    return runtime;
}

void runtime_run(runtime_t* runtime, coroutine_fn_t main_coroutine_fn)
{
    thread_pool_run(runtime->thread_pool, __thread_scheduler);
    coroutines_queue_push(runtime->global_queue, main_coroutine_fn);
    thread_pool_wait(runtime->thread_pool);
}

void runtime_destroy(runtime_t* runtime)
{
    thread_pool_destroy(runtime->thread_pool);
    coroutines_queue_destroy(runtime->global_queue);
}