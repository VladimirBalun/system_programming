#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

typedef void (*coroutine_fn_t)();
typedef struct scheduler_t scheduler_t;

extern scheduler_t* g_scheduler;

scheduler_t* scheduler_init();
void scheduler_run(scheduler_t*, coroutine_fn_t);
void scheduler_add_coroutine(scheduler_t*, coroutine_fn_t); // schedule, spawn
void scheduler_park_coroutine(scheduler_t*);
void scheduler_destroy(scheduler_t*);

#endif // __SCHEDULER_H__