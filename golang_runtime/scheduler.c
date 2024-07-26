#include "scheduler.h"

#include "utils.h"
#include "coroutine.h"
#include "coroutines_queue.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct scheduler_t {
    coroutines_queue_t* global_queue;
    coroutine_t*        running_coroutine;
    coroutine_t*        scheduling_coroutine;
} scheduler_t;

static void __schedule_coroutines(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] incorrect scheduler\n", __FILE__, __LINE__);
        return;
    }

    coroutine_t* coroutine = coroutines_queue_pop(scheduler->global_queue);
    while (coroutine) {
        scheduler->running_coroutine = coroutine;
        coroutine_set_scheduler(coroutine, scheduler);
        coroutine_switch(scheduler->scheduling_coroutine, coroutine);

        if (coroutine_status(coroutine) == STATUS_STOPPED)
            coroutine_destroy(&coroutine);

        coroutine = coroutines_queue_pop(scheduler->global_queue);
    }
}

scheduler_t* scheduler_init(coroutines_queue_t* queue)
{
    if (!queue) {
        fprintf(stderr, "[%s:%d] incorrect queue\n", __FILE__, __LINE__);
        return NULL;   
    }

    scheduler_t* scheduler = malloc(sizeof(scheduler_t));
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for scheduler\n", __FILE__, __LINE__);
        return NULL;        
    }

    scheduler->global_queue = queue;
    scheduler->running_coroutine = NULL;
    scheduler->scheduling_coroutine = NULL;
    return scheduler;
}

coroutine_t* scheduler_get_scheduling_coroutine(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] incorrect scheduler\n", __FILE__, __LINE__);
        return NULL;
    }

    return scheduler->scheduling_coroutine;
}

void scheduler_run(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] incorrect scheduler\n", __FILE__, __LINE__);
        return;
    }

    coroutine_t* current_coroutine = coroutine_make_current_context();
    if (!current_coroutine) {
        fprintf(stderr, "[%s:%d] failed to make current coroutine\n", __FILE__, __LINE__);
        return;       
    }

    scheduler->scheduling_coroutine = current_coroutine;
    __schedule_coroutines(scheduler);
}

void scheduler_spawn_coroutine(scheduler_t* scheduler, coroutine_fn_t function)
{
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] incorrect scheduler\n", __FILE__, __LINE__);
        return;
    }

    if (!function) {
        fprintf(stderr, "[%s:%d] incorrect function\n", __FILE__, __LINE__);
        return;        
    }

    coroutine_t* coroutine = coroutine_init(function);
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] failed to create coroutine\n", __FILE__, __LINE__);
        return;
    }

    coroutines_queue_push(scheduler->global_queue, coroutine);
}

void scheduler_park_coroutine(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] incorrect scheduler\n", __FILE__, __LINE__);
        return;
    }

    coroutine_t* running_coroutine = scheduler->running_coroutine;
    scheduler->running_coroutine = NULL;

    coroutine_set_scheduler(running_coroutine, NULL);
    coroutines_queue_push(scheduler->global_queue, running_coroutine);
    coroutine_switch(running_coroutine, scheduler->scheduling_coroutine);
}

void scheduler_destroy(scheduler_t* scheduler)
{
    if (!scheduler) {
        fprintf(stderr, "[%s:%d] incorrect scheduler\n", __FILE__, __LINE__);
        return;
    }

    coroutine_destroy(&scheduler->scheduling_coroutine);
    SAFE_DELETE(scheduler);
}