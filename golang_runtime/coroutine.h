#ifndef __COROUTINE_H__
#define __COROUTINE_H__

typedef struct scheduler_t scheduler_t;
typedef struct coroutine_t coroutine_t;

typedef void (*coroutine_fn_t)(coroutine_t*);

typedef enum coroutine_status_t {
    STATUS_RUNNING,
    STATUS_RUNNABLE,
    STATUS_STOPPED,
} coroutine_status_t;

coroutine_t* coroutine_make_current_context();
coroutine_t* coroutine_init(coroutine_fn_t);
coroutine_status_t coroutine_status(coroutine_t*);
void coroutine_switch(coroutine_t*, coroutine_t*);
void coroutine_stop(coroutine_t*);
void coroutine_destroy(coroutine_t**);

void coroutine_set_scheduler(coroutine_t*, scheduler_t*);
scheduler_t* coroutine_get_scheduler(coroutine_t*);

#endif // __COROUTINE_H__