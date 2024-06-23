#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

typedef struct scheduler_t scheduler_t;
typedef struct thread_pool_t thread_pool_t;

typedef void (*thread_fn_t)(void*);

thread_pool_t* thread_pool_init(int);
void thread_pool_run(thread_pool_t*);
void thread_pool_submit(thread_pool_t*, thread_fn_t);
void thread_pool_wait(thread_pool_t*);
void thread_pool_destroy(thread_pool_t*);

#endif // __THREAD_POOL_H__