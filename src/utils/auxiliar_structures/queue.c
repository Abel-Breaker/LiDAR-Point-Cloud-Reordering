#include "queue.h"
#include "../error_handler.h"
#include <stdlib.h>

struct Queue{
    size_t head;
    size_t tail;
    size_t *indices;
};

// Crear cola
Queue *createQueue(size_t size) {
    Queue* q = calloc(1, sizeof(*q)); // Set head and tail to 0
    if(!q){
        handle_error(ERROR_MALLOC, ERR_FATAL, "Can not allocate memory for queue");
        return NULL; // Only to avoid warning, never reached
    }
    q->indices = malloc(sizeof(*(q->indices)) * size);
    if(!q->indices){
        free(q); 
        handle_error(ERROR_MALLOC, ERR_FATAL, "Can not allocate memory for queue");
        return NULL; // Only to avoid warning, never reached
    }

    return q;
}

// Encolar (push)
void enqueue(Queue* q, size_t index) {
    q->indices[q->tail++] = index;
}

// TODO: Test to reallocate each X dequeues to reduce memory usage and check impact on rendimiento
size_t dequeue(Queue* q) {
    /*
    if(q->tail % 100000 == 0){ // Each 100000 dequeues
        reallocate
    }
    */
   return q->indices[q->head++];
}

bool is_queue_empty(const Queue* q){
    return q->tail == q->head;
}

void destroyQueue(Queue *q) {
    q->head = q->tail = 0;
    free(q->indices);
    free(q);
}