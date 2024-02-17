#include "coroutine.h"
#include "scheduler.h"

#define GO_RUNTIME(function) \
    g_scheduler = scheduler_init(); \
    scheduler_run(g_scheduler, (function)); \
    scheduler_destroy(g_scheduler)

#define GO(function) \
    scheduler_add_coroutine(g_scheduler, (function))

#define PRINT(text) \
    printf((text)); \
    scheduler_park_coroutine(g_scheduler)

void coroutine2()
{
    PRINT("coroutine2 started\n");
    PRINT("coroutine2 finished\n");
}

void coroutine1()
{
    PRINT("coroutine1 started\n");
    PRINT("coroutine1 finished\n");
}

void main_coroutine()
{
    PRINT("main coroutine started\n");
    GO(coroutine1);
    PRINT("main middleware\n");
    GO(coroutine2);
    PRINT("main coroutine finished\n");
}
 
int main()
{  
    GO_RUNTIME(main_coroutine);
    return EXIT_SUCCESS;
}