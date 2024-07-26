#include "thread_pool.h"

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct thread_pool_t {
    pthread_t* threads;
    int        last_thread_idx;
    int        threads_number;
} thread_pool_t;

thread_pool_t* thread_pool_init(int threads_number)
{
    thread_pool_t* thread_pool = malloc(sizeof(thread_pool_t));
    if (!thread_pool) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for thread_pool\n", __FILE__, __LINE__);
        return NULL;
    }

    pthread_t* threads = malloc(sizeof(pthread_t) * threads_number);
    if (!threads) {
        free(thread_pool);
        fprintf(stderr, "[%s:%d] failed to allocate memory for threads\n", __FILE__, __LINE__);
        return NULL;
    }

    thread_pool->threads = threads;
    thread_pool->last_thread_idx = 0;
    thread_pool->threads_number = threads_number;
    return thread_pool;
}

int thread_pool_threads_number(thread_pool_t* thread_pool)
{
    if (!thread_pool) {
        fprintf(stderr, "[%s:%d] incorrect thread_pool\n", __FILE__, __LINE__);
        return 0;
    }

    return thread_pool->threads_number;
}

void thread_pool_run_thread(thread_pool_t* thread_pool, thread_fn_t thread_fn, void* parameter)
{
    if (!thread_pool) {
        fprintf(stderr, "[%s:%d] incorrect thread_pool\n", __FILE__, __LINE__);
        return;
    }

    if (!thread_fn) {
        fprintf(stderr, "[%s:%d] incorrect thread_fn\n", __FILE__, __LINE__);
        return;
    }

    if (thread_pool->last_thread_idx == thread_pool->threads_number) {
        fprintf(stderr, "[%s:%d] threads number overflow\n", __FILE__, __LINE__);
        return;
    }

    int thread_idx = thread_pool->last_thread_idx++;
    if (pthread_create(&thread_pool->threads[thread_idx], NULL, thread_fn, parameter) != 0)
        fprintf(stderr, "[%s:%d] failed to create thread\n", __FILE__, __LINE__);
}

void thread_pool_wait_threads(thread_pool_t* thread_pool)
{
    if (!thread_pool) {
        fprintf(stderr, "[%s:%d] incorrect thread_pool\n", __FILE__, __LINE__);
        return;
    }

    for (int idx = 0; idx < thread_pool->last_thread_idx; ++idx) {
        if (pthread_join(thread_pool->threads[idx], NULL) != 0)
            fprintf(stderr, "[%s:%d] failed to join thread\n", __FILE__, __LINE__);
    }
}

void thread_pool_destroy(thread_pool_t** thread_pool)
{
    if (!thread_pool || !*thread_pool) {
        fprintf(stderr, "[%s:%d] incorrect thread_pool\n", __FILE__, __LINE__);
        return;
    }

    SAFE_DELETE((*thread_pool)->threads);
    SAFE_DELETE((*thread_pool));
}