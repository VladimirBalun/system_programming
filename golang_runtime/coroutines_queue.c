#include "coroutines_queue.h"

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
} coroutines_queue_t;

coroutines_queue_t* coroutines_queue_init()
{
    coroutines_queue_t* queue = malloc(sizeof(coroutines_queue_t));
    if (!queue) {
        fprintf(stderr, "failed to allocate memory for queue\n");
        return NULL;
    }

    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue);
        fprintf(stderr, "failed to init mutex for queue\n");
        return NULL;
    } 

    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void coroutines_queue_push(coroutines_queue_t* queue, coroutine_t* coroutine)
{
    if (!queue) {
        fprintf(stderr, "incorrect queue\n");
        return;
    }

    if (!coroutine) {
        fprintf(stderr, "incorrect coroutine\n");
        return;
    }

    node_t* node = malloc(sizeof(node_t));
    if (!node) {
        fprintf(stderr, "failed to allocate memory for node\n");
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
        fprintf(stderr, "incorrect queue\n");
        return NULL;
    }
    
    pthread_mutex_lock(&queue->mutex);

    if (!queue->head) {
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    }

    while (!queue->head)
        pthread_cond_wait(&queue->not_empty, &queue->mutex);

    node_t* temp_node = queue->head;
    coroutine_t* coroutine = temp_node->coroutine;
    queue->head = queue->head->next;
    if (!queue->head)
        queue->tail = NULL;
    
    pthread_mutex_unlock(&queue->mutex);

    free(temp_node);
    return coroutine;
}

void coroutines_queue_destroy(coroutines_queue_t* queue)
{
    if (!queue) {
        fprintf(stderr, "incorrect queue\n");
        return;
    }

    node_t* iterator = queue->head;
    while (iterator) {
        node_t* temp = iterator;
        iterator = iterator->next;
        coroutine_destroy(temp->coroutine);
        free(temp);
    }

    pthread_mutex_destroy(&queue->mutex); 
    free(queue);
}