#pragma once
#include <stddef.h>
/**
 * This is a highly efficient implementation of a queue. It has no safety checks for sizes, overflows, etc.
 * Therefore, it is the programmer's responsibility to use it correctly.
 */

typedef struct Queue Queue;

Queue *create_queue();

void enqueue(Queue *q, size_t index);

size_t dequeue(Queue *q);

bool is_queue_empty(const Queue *q);

size_t get_num_elements(const Queue *q);

void destroy_queue(Queue *q);