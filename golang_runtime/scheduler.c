#include "scheduler.h"

#include "coroutine.h"
#include "coroutines_queue.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct scheduler_t {
    coroutines_queue_t* global_queue;
    coroutine_t*        running_coroutine;
    coroutine_t*        scheduling_coroutine;
} scheduler_t;

static void __sheduling_loop(scheduler_t*, coroutine_fn_t);
static void __coroutine_trampline(scheduler_t*, coroutine_fn_t);

scheduler_t* scheduler_init(coroutines_queue_t* queue)
{
    scheduler_t* scheduler = malloc(sizeof(scheduler_t));
    if (!scheduler) {
        coroutines_queue_destroy(queue);
        fprintf(stderr, "failed to allocate memory for scheuler\n");
        return NULL;        
    }

    scheduler->global_queue = queue;
    return scheduler;
}

void scheduler_run(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    coroutine_t* scheduling_coroutine = coroutine_init(scheduler, __sheduling_loop, NULL);
    if (!scheduling_coroutine) {
        fprintf(stderr, "failed to create scheduling coroutine\n");
        return;        
    }

    scheduler->scheduling_coroutine = scheduling_coroutine;
    coroutine_run(scheduling_coroutine);
}

void scheduler_spawn_coroutine(scheduler_t* scheduler, coroutine_fn_t function)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    if (!function) {
        fprintf(stderr, "incorrect function\n");
        return;        
    }

    coroutine_t* coroutine = coroutine_init(scheduler, __coroutine_trampline, function);
    if (!coroutine) {
        fprintf(stderr, "failed to create coroutine\n");
        return;
    }

    coroutines_queue_push(scheduler->global_queue, coroutine);
}

void scheduler_park_coroutine(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    coroutines_queue_push(scheduler->global_queue, scheduler->running_coroutine);
    coroutine_switch(scheduler->running_coroutine, scheduler->scheduling_coroutine);
}

void scheduler_destroy(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduler\n");
        return;
    }

    free(scheduler);
}

static void __sheduling_loop(scheduler_t* scheduler, coroutine_fn_t)
{
    coroutine_t* coroutine = coroutines_queue_pop(scheduler->global_queue);
    while (coroutine) {
        scheduler->running_coroutine = coroutine;
        coroutine_switch(scheduler->scheduling_coroutine, scheduler->running_coroutine);

        if (coroutine_status(coroutine) == STATUS_STOPPED)
            coroutine_destroy(coroutine);

        coroutine = coroutines_queue_pop(scheduler->global_queue);
    }
}

static void __coroutine_trampline(scheduler_t* scheduler, coroutine_fn_t function)
{
    if (!function) {
        fprintf(stderr, "incorrect function\n");
        return;        
    }

    function(scheduler);

    coroutine_stop(scheduler->running_coroutine);
    coroutine_switch(scheduler->running_coroutine, scheduler->scheduling_coroutine);
}