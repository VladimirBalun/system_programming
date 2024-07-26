#ifndef __COROUTINES_QUEUE_H__
#define __COROUTINES_QUEUE_H__

typedef struct coroutine_t coroutine_t;
typedef struct coroutines_queue_t coroutines_queue_t;

coroutines_queue_t* coroutines_queue_init(int);
void coroutines_queue_push(coroutines_queue_t*, coroutine_t*);
coroutine_t* coroutines_queue_pop(coroutines_queue_t*);
void coroutines_queue_destroy(coroutines_queue_t**);

#endif // __COROUTINES_QUEUE_H__