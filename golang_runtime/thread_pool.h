#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

typedef struct thread_pool_t thread_pool_t;

typedef void* (*thread_fn_t)(void*);

thread_pool_t* thread_pool_init(int);
int thread_pool_threads_number(thread_pool_t*);
void thread_pool_run_thread(thread_pool_t*, thread_fn_t, void*);
void thread_pool_wait_threads(thread_pool_t*);
void thread_pool_destroy(thread_pool_t**);

#endif // __THREAD_POOL_H__