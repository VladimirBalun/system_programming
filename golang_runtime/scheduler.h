#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

typedef struct scheduler_t scheduler_t;
typedef struct coroutines_queue_t coroutines_queue_t;

typedef void (*coroutine_fn_t)(scheduler_t*);

scheduler_t* scheduler_init(coroutines_queue_t*);
void scheduler_run(scheduler_t*);
void scheduler_spawn_coroutine(scheduler_t*, coroutine_fn_t);
void scheduler_park_coroutine(scheduler_t*);
void scheduler_destroy(scheduler_t*);

#endif // __SCHEDULER_H__