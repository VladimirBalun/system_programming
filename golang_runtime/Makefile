PROGRAM_NAME = golang_runtime
LD_FLAGS = -lpthread
GCC_FLAGS = -std=c99 -Wextra -Werror -Wall -Wno-gnu-folding-constant 

all: scheduler.c coroutine.c coroutines_queue.c thread_pool.c runtime.c main.c
	gcc $(GCC_FLAGS) $(LD_FLAGS) scheduler.c coroutine.c coroutines_queue.c thread_pool.c runtime.c main.c -o $(PROGRAM_NAME)

clean:
	rm $(PROGRAM_NAME)