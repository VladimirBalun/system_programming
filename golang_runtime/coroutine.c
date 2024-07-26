#include "coroutine.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ucontext.h>

#include "utils.h"
#include "scheduler.h"

#define STACK_SIZE (1 << 20) * 2

typedef struct coroutine_t {
    scheduler_t*       scheduler;
    ucontext_t*        context;
    coroutine_status_t status;
} coroutine_t;

static ucontext_t* __create_context();
static void __coroutine_trampline(coroutine_t*, coroutine_fn_t);

coroutine_t* coroutine_init(coroutine_fn_t coroutine_fn)
{
    ucontext_t* context = __create_context();
    if (!context) {
        fprintf(stderr, "[%s:%d] failed to create coroutine context\n", __FILE__, __LINE__);
        return NULL;
    }

    coroutine_t* coroutine = malloc(sizeof(coroutine_t));
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for coroutine\n", __FILE__, __LINE__);
        return NULL;
    }

    coroutine->scheduler = NULL;
    coroutine->context = context;
    coroutine->status = STATUS_RUNNABLE;
    makecontext(coroutine->context, (void (*)())__coroutine_trampline, 2, coroutine, coroutine_fn);

    return coroutine;
}

void coroutine_set_scheduler(coroutine_t* coroutine, scheduler_t* scheduler)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    coroutine->scheduler = scheduler;
}

scheduler_t* coroutine_get_scheduler(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return NULL;
    }

    return coroutine->scheduler;
}

coroutine_t* coroutine_make_current_context()
{
    ucontext_t* context = __create_context();
    if (!context) {
        fprintf(stderr, "[%s:%d] failed to create coroutine context\n", __FILE__, __LINE__);
        return NULL;
    }

    if (getcontext(context) < 0) {
        fprintf(stderr, "[%s:%d] failed to initialize context\n", __FILE__, __LINE__);
        return NULL;
    }

    coroutine_t* coroutine = malloc(sizeof(coroutine_t));
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for coroutine\n", __FILE__, __LINE__);
        return NULL;
    }

    coroutine->scheduler = NULL;
    coroutine->context = context;
    coroutine->status = STATUS_RUNNABLE;
    return coroutine;
}

coroutine_status_t coroutine_status(coroutine_t* coroutine)
{
    if (!coroutine)
        return STATUS_STOPPED;

    return coroutine->status;
}

void coroutine_switch(coroutine_t* from_coroutine, coroutine_t* to_coroutine)
{
    if (!from_coroutine || !to_coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutines\n", __FILE__, __LINE__);
        return;
    }

    if (from_coroutine->status != STATUS_STOPPED)
        from_coroutine->status = STATUS_RUNNABLE;

    to_coroutine->status = STATUS_RUNNING;
    if (swapcontext(from_coroutine->context, to_coroutine->context) < 0)
        fprintf(stderr, "[%s:%d] failed to swap context\n", __FILE__, __LINE__);
}

void coroutine_stop(coroutine_t* coroutine)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    coroutine->status = STATUS_STOPPED;
}

void coroutine_destroy(coroutine_t** coroutine)
{
    if (!coroutine || !(*coroutine)) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    SAFE_DELETE((*coroutine)->context->uc_stack.ss_sp);
    SAFE_DELETE((*coroutine)->context);
    SAFE_DELETE(*coroutine);
}

static ucontext_t* __create_context()
{
    void* stack = malloc(STACK_SIZE);
    if (!stack) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for stack\n", __FILE__, __LINE__);
        return NULL;
    }

    ucontext_t* context = malloc(sizeof(ucontext_t));
    if (!context) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for context\n", __FILE__, __LINE__);
        free(stack);
        return NULL;
    }

    if (getcontext(context) < 0) {
        fprintf(stderr, "[%s:%d] failed to initialize context\n", __FILE__, __LINE__);
        free(stack);
        free(context);
        return NULL;
    }

    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_stack.ss_flags = 0;
    
    return context;
}

static void __coroutine_trampline(coroutine_t* coroutine, coroutine_fn_t function)
{
    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;        
    }

    if (!function) {
        fprintf(stderr, "[%s:%d] incorrect function\n", __FILE__, __LINE__);
        return;        
    }

    function(coroutine);

    coroutine_stop(coroutine);
    coroutine_switch(coroutine, scheduler_get_scheduling_coroutine(coroutine->scheduler));
}