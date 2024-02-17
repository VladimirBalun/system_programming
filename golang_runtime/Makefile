PROGRAM_NAME = golang_runtime
GCC_FLAGS = -Wextra -Werror -Wall -Wno-gnu-folding-constant

all: scheduler.c coroutine.c coroutines_queue.c main.c
	gcc $(GCC_FLAGS) scheduler.c coroutine.c coroutines_queue.c main.c -o $(PROGRAM_NAME)

clean:
	rm $(PROGRAM_NAME)