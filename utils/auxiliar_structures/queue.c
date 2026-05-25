#include "queue.h"
#include "../error_handler.h"
#include <stdlib.h>

#define QUEUE_INITIAL_SIZE 10000
#define QUEUE_GROWTH_SIZE 10000

struct Queue {
	index_t head;
	index_t tail;
	index_t *indices;
	index_t num_elements;
	index_t capacity;
};

// Crear cola
Queue *create_queue(void)
{
	Queue *q = calloc(1, sizeof(*q));
	if (!q) {
		handle_error(ERROR_MALLOC, ERR_FATAL, "Can not allocate memory for queue");
		return NULL;
	}
	q->indices = malloc(sizeof(*(q->indices)) * QUEUE_INITIAL_SIZE);
	if (!q->indices) {
		free(q);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Can not allocate memory for queue");
		return NULL;
	}
	q->capacity = QUEUE_INITIAL_SIZE;
	return q;
}

static void resize_queue(Queue *q)
{
	index_t new_capacity = q->capacity + QUEUE_GROWTH_SIZE;
	index_t *new_indices = realloc(q->indices, sizeof(*(q->indices)) * new_capacity);
	if (!new_indices) {
		handle_error(ERROR_MALLOC, ERR_FATAL, "Can not reallocate memory for queue");
		return;
	}
	q->indices = new_indices;
	q->capacity = new_capacity;
}

void enqueue(Queue *q, index_t index)
{
	if (q->tail >= q->capacity) {
        resize_queue(q);
	}

	q->indices[q->tail++] = index;
	q->num_elements++;
}

index_t dequeue(Queue *q)
{
	q->num_elements--;
	return q->indices[q->head++];
}

index_t get_num_elements(const Queue *q)
{
	return q->num_elements;
}

bool is_queue_empty(const Queue *q)
{
	return q->tail == q->head;
}

void destroy_queue(Queue *q)
{
	q->head = q->tail = 0;
	free(q->indices);
	free(q);
}