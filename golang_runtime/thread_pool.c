#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#define MIN_THREADS_NUMBER     1
#define MAX_THREADS_NUMBER     64
#define DEFAULT_THREADS_NUMBER 4

typedef struct thread_pool_t {
    pthread_t* threads;
    int        threads_number;
} thread_pool_t;

thread_pool_t* thread_pool_init(int threads_number)
{
    if (threads_number < MIN_THREADS_NUMBER || threads_number > MAX_THREADS_NUMBER)
        threads_number = DEFAULT_THREADS_NUMBER;

    thread_pool_t* thread_pool = malloc(sizeof(thread_pool_t));
    if (!thread_pool) {
        fprintf(stderr, "failed to allocate memory for thread_pool\n");
        return NULL;
    }

    pthread_t* threads = malloc(sizeof(pthread_t) * threads_number);
    if (!threads) {
        free(thread_pool);
        fprintf(stderr, "failed to allocate memory for threads\n");
        return NULL;
    }

    thread_pool->threads_number = threads_number;
    return thread_pool;
}

void thread_pool_run(thread_pool_t* thread_pool, thread_fn_t thread_fn)
{
    for (int i = 0; i < thread_pool->threads_number; ++i) {
        if (pthread_create(&thread_pool->threads[i], NULL, thread_fn, NULL) != 0)
            fprintf(stderr, "failed to create thread\n");
    }
}

void thread_pool_wait(thread_pool_t* thread_pool)
{
    for (int i = 0; i < thread_pool->threads_number; ++i) {
        if (pthread_join(thread_pool->threads[i], NULL) != 0)
            fprintf(stderr, "failed to join thread\n");
    }
}

void thread_pool_destroy(thread_pool_t* thread_pool)
{
    if (!thread_pool) {
        fprintf(stderr, "incorrect thread_pool\n");
        return;
    }

    free(thread_pool->threads);
    free(thread_pool);
}