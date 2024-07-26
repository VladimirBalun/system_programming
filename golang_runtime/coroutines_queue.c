#include "coroutines_queue.h"

#include "utils.h"
#include "coroutine.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node_t node_t;
typedef struct node_t {
    coroutine_t* coroutine;
    node_t*      next;
} node_t;

typedef struct coroutines_queue_t {
    node_t*         head;
    node_t*         tail;

    pthread_mutex_t mutex;
    pthread_cond_t  not_empty;

    int             waiting_number;
    int             waiting_max_number;
    int             need_to_wait;
} coroutines_queue_t;

coroutines_queue_t* coroutines_queue_init(int waiting_max_number)
{
    coroutines_queue_t* queue = malloc(sizeof(coroutines_queue_t));
    if (!queue) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for queue\n", __FILE__, __LINE__);
        return NULL;
    }

    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue);
        fprintf(stderr, "[%s:%d] failed to init mutex for queue\n", __FILE__, __LINE__);
        return NULL;
    }

    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex); 
        free(queue);
        fprintf(stderr, "[%s:%d] failed to init cond for queue\n", __FILE__, __LINE__);
        return NULL;
    } 

    queue->head = NULL;
    queue->tail = NULL;
    queue->waiting_number = 0;
    queue->waiting_max_number = waiting_max_number;
    queue->need_to_wait = TRUE;
    return queue;
}

void coroutines_queue_push(coroutines_queue_t* queue, coroutine_t* coroutine)
{
    if (!queue) {
        fprintf(stderr, "[%s:%d] incorrect queue\n", __FILE__, __LINE__);
        return;
    }

    if (!coroutine) {
        fprintf(stderr, "[%s:%d] incorrect coroutine\n", __FILE__, __LINE__);
        return;
    }

    node_t* node = malloc(sizeof(node_t));
    if (!node) {
        fprintf(stderr, "[%s:%d] failed to allocate memory for node\n", __FILE__, __LINE__);
        return;       
    }

    node->coroutine = coroutine;
    node->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    if (!queue->head) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }

    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_signal(&queue->not_empty); 
}

coroutine_t* coroutines_queue_pop(coroutines_queue_t* queue)
{
    if (!queue) {
        fprintf(stderr, "[%s:%d] incorrect queue\n", __FILE__, __LINE__);
        return NULL;
    }
    
    pthread_mutex_lock(&queue->mutex);

    while (!queue->head && queue->need_to_wait == TRUE) {
        ++queue->waiting_number;
        if (queue->waiting_number == queue->waiting_max_number) {
            queue->need_to_wait = FALSE;
            pthread_cond_broadcast(&queue->not_empty); 
            break;
        }

        pthread_cond_wait(&queue->not_empty, &queue->mutex);
        --queue->waiting_number;
    }

    if (!queue->head) {
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    }

    node_t* temp_node = queue->head;
    coroutine_t* coroutine = temp_node->coroutine;
    queue->head = queue->head->next;
    if (!queue->head)
        queue->tail = NULL;
    
    pthread_mutex_unlock(&queue->mutex);

    SAFE_DELETE(temp_node);
    return coroutine;
}

void coroutines_queue_destroy(coroutines_queue_t** queue)
{
    if (!queue || !(*queue)) {
        fprintf(stderr, "[%s:%d] incorrect queue\n", __FILE__, __LINE__);
        return;
    }

    node_t* iterator = (*queue)->head;
    while (iterator) {
        node_t* temp = iterator;
        iterator = iterator->next;
        coroutine_destroy(&temp->coroutine);
        SAFE_DELETE(temp);
    }

    pthread_cond_destroy(&(*queue)->not_empty);
    pthread_mutex_destroy(&(*queue)->mutex);
    SAFE_DELETE(*queue);
}