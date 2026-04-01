#pragma once
/**
 * This is a highly efficient implementation of a queue. It has no safety checks for sizes, overflows, etc.
 * Therefore, it is the programmer's responsibility to use it correctly.
 */

typedef struct Queue Queue;

// Crear cola
Queue *createQueue(unsigned int size);

// Encolar (push)
void enqueue(Queue *q, unsigned int index);

unsigned int dequeue(Queue *q);

bool is_queue_empty(const Queue *q);

void destroyQueue(Queue *q);