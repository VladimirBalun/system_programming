#include "scheduler.h"

#include "coroutine.h"
#include "coroutines_queue.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct scheduler_t {
    coroutines_queue_t* queue;
    coroutine_t*        running_coroutine;
    coroutine_t*        scheduling_coroutine;
} scheduler_t;

scheduler_t* g_scheduler;

scheduler_t* scheduler_init()
{
    coroutines_queue_t* queue = coroutines_queue_init();
    if (!queue) {
        fprintf(stderr, "failed to create coroutines queue\n");
        return NULL;
    }

    scheduler_t* scheduler = malloc(sizeof(scheduler_t));
    if (!scheduler) {
        fprintf(stderr, "failed to allocate memory for scheuler\n");
        return NULL;        
    }

    scheduler->queue = queue;
    return scheduler;
}

void scheduler_run(scheduler_t* scheduler, coroutine_fn_t function)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    if (!function) {
        fprintf(stderr, "incorrect function\n");
        return;        
    }

    coroutine_t* main_coroutine = coroutine_init(function);
    if (!main_coroutine) {
        fprintf(stderr, "failed to create main coroutine\n");
        return;        
    }

    scheduler->running_coroutine = main_coroutine;
    coroutine_run(main_coroutine);
}

void scheduler_add_coroutine(scheduler_t* scheduler, coroutine_fn_t function)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    if (!function) {
        fprintf(stderr, "incorrect function\n");
        return;        
    }

    coroutine_t* coroutine = coroutine_init(function);
    if (!coroutine) {
        fprintf(stderr, "failed to create coroutine\n");
        return;        
    }

    coroutines_queue_push(scheduler->queue, coroutine);
}

void scheduler_park_coroutine(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    coroutine_t* coroutine = coroutines_queue_pop(scheduler->queue);
    if (!coroutine)
        return;

    coroutine_t* previous_coroutine = scheduler->running_coroutine;
    coroutines_queue_push(scheduler->queue, scheduler->running_coroutine);
    scheduler->running_coroutine = coroutine;
    coroutine_swap(previous_coroutine, coroutine);
}

void scheduler_destroy(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    coroutines_queue_destroy(scheduler->queue);
    free(scheduler);
}
