#ifndef __RUNTIME_H__
#define __RUNTIME_H__

typedef struct runtime_t runtime_t;
typedef struct coroutine_t coroutine_t;

typedef void (*coroutine_fn_t)(coroutine_t*);

runtime_t* runtime_init(int);
void runtime_run(runtime_t*, coroutine_fn_t);
void runtime_destroy(runtime_t**);

#endif // __RUNTIME_H__