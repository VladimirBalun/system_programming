#ifndef __COROUTINE_H__
#define __COROUTINE_H__

typedef void (*coroutine_fn_t)();
typedef struct coroutine_t coroutine_t;

coroutine_t* coroutine_init(coroutine_fn_t);
void coroutine_run(coroutine_t*);
void coroutine_swap(coroutine_t*, coroutine_t*);
void coroutine_destroy(coroutine_t*);

#endif // __COROUTINE_H__