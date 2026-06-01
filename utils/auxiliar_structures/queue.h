#pragma once
#include "../types.h"
/**
 * This is a highly efficient implementation of a queue. It has no safety checks for sizes, overflows, etc.
 * Therefore, it is the programmer's responsibility to use it correctly.
 */

typedef struct Queue Queue;

/**
 * @brief Creates and initializes a new queue.
 *
 * @return Pointer to the newly created queue, or NULL on failure.
 */
Queue *create_queue();

/**
 * @brief Adds an element to the end of the queue.
 *
 * @param q Pointer to the queue.
 * @param index Element to enqueue.
 */
void enqueue(Queue *q, index_t index);

/**
 * @brief Removes and returns the element at the front of the queue.
 *
 * @param q Pointer to the queue.
 * 
 * @return The dequeued element.
 */
index_t dequeue(Queue *q);

/**
 * @brief Checks whether the queue is empty.
 *
 * @param q Pointer to the queue.
 * 
 * @return true if the queue contains no elements, false otherwise.
 */
bool is_queue_empty(const Queue *q);

/**
 * @brief Returns the number of elements currently stored in the queue.
 *
 * @param q Pointer to the queue.
 * @return Number of elements in the queue.
 */
index_t get_num_elements(const Queue *q);

/**
 * @brief Releases all resources associated with the queue.
 *
 * @param q Pointer to the queue.
 */
void destroy_queue(Queue *q);