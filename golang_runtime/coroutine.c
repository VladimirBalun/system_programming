#include "coroutine.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ucontext.h>

#define STACK_SIZE 2 << 20

typedef struct coroutine_t {
    ucontext_t*        context;
    coroutine_status_t status;
} coroutine_t;

static ucontext_t* __create_context();

coroutine_t* coroutine_init(scheduler_t* scheduler, coroutine_trampline_fn_t coroutine_trampline_fn, coroutine_fn_t coroutine_fn)
{
    if (!scheduler) {
        fprintf(stderr, "incorrect scheduller\n");
        return NULL;       
    }

    if (!coroutine_trampline_fn) {
        fprintf(stderr, "incorrect function\n");
        return NULL;
    }

    ucontext_t* context = __create_context();
    if (!context) {
        fprintf(stderr, "failed to create coroutine context\n");
        return NULL;
    }

    coroutine_t* coroutine = malloc(sizeof(coroutine_t));
    if (!coroutine) {
        fprintf(stderr, "failed to allocate memory for coroutine\n");
        return NULL;
    }

    coroutine->context = context;
    coroutine->status = STATUS_RUNNABLE;
    makecontext(coroutine->context, (void (*)())coroutine_trampline_fn, 2, scheduler, coroutine_fn);

    return coroutine;
}

void coroutine_run(coroutine_t* coroutine)
{
    if (!coroutine)
        return;

    coroutine->status = STATUS_RUNNING;
    setcontext(coroutine->context);
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
        fprintf(stderr, "incorrect coroutines\n");
        return;
    }

    if (from_coroutine->status != STATUS_STOPPED)
        from_coroutine->status = STATUS_RUNNABLE;

    to_coroutine->status = STATUS_RUNNING;
    if (swapcontext(from_coroutine->context, to_coroutine->context) < 0) {
        fprintf(stderr, "failed to swap context\n");
    }
}

void coroutine_stop(coroutine_t* coroutine)
{
    if (!coroutine)
        return;

    coroutine->status = STATUS_STOPPED;
}

void coroutine_destroy(coroutine_t* coroutine)
{
    if (!coroutine)
        return;

    free(coroutine->context->uc_stack.ss_sp);
    free(coroutine->context);
    free(coroutine);
}

static ucontext_t* __create_context()
{
    void* stack = malloc(STACK_SIZE);
    if (!stack) {
        fprintf(stderr, "failed to allocate memory for stack\n");
        return NULL;
    }

    ucontext_t* context = malloc(sizeof(ucontext_t));
    if (!context) {
        fprintf(stderr, "failed to allocate memory for context\n");
        free(stack);
        return NULL;
    }

    if (getcontext(context) < 0) {
        fprintf(stderr, "failed to initialize context\n");
        free(stack);
        free(context);
        return NULL;
    }

    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = STACK_SIZE;
    context->uc_stack.ss_flags = 0;
    
    return context;
}