#include "BFS.h"

#include "../utils/auxiliar_structures/queue.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * TODO: Probar distintos criterios añadir los nodos a la cola:
 *  - Por menor distancia euclídea
 *  - Por orden creciente en base al número de índice
 *  - Por menor distanca euclídea pero invirtiendo la permitación (equivalente a RCM)
 *  - Probar DFS (utilizar una pila/recursión en vez de una cola)
 *  - Simetrizar la matriz
 */
/*
void reorder_bfs_sort_by_distance(const KDTree *tree, const Points *points, Points *new_points)
{
	unsigned int number_of_points = (unsigned int)tree->pts->num_points;

	Queue *queue = createQueue(number_of_points);

	unsigned int *permutations;
	bool *visited;
	permutations = malloc(number_of_points * sizeof(*permutations));
	visited = calloc(number_of_points, sizeof(*visited));

	unsigned int points_visited = 0;

    // Makes sure to visit all the points, even if they are not anyone's neighbors (not recheable)
	for (unsigned int i = 0; i < points->num_points; ++i) {
		if (visited[i] == true)
			continue;

		enqueue(queue, i);

		while (is_queue_empty(queue) == false) {
			unsigned int new_index = dequeue(queue);

			permutations[points_visited] = new_index;
			++points_visited;

			visited[new_index] = true;

			size_t neighbours[K];
			double neighbours_distances[K];
			start_kdtree_knearest(tree, new_index, neighbours, neighbours_distances);

			for (unsigned int j = 0; j < K; ++j) {
				if (!visited[neighbours[j]]) {
					visited[neighbours[j]] = true;
					enqueue(queue, (unsigned int)neighbours[j]);
				}
			}
		}
	}

	if (!reserve_memory_points(new_points, points->num_points)) {
		destroyQueue(queue);
		free(permutations);
		free(visited);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	for(int i=0; i<200; i++){
		//printf("%d ", permutations[i]);
	}
	

	for (unsigned int i = 0; i < points_visited; ++i) { // points_visited == points->num_points
		add_point(new_points, i, points->x[permutations[i]], points->y[permutations[i]],
			  points->z[permutations[i]]);
	}

	destroyQueue(queue);
	free(permutations);
	free(visited);
}

void reorder_bfs_sort_by_distance_reverse(const KDTree *tree, const Points *points, Points *new_points)
{
	unsigned int number_of_points = (unsigned int)tree->pts->num_points;

	Queue *queue = createQueue(number_of_points);

	unsigned int *permutations;
	bool *visited;
	permutations = malloc(number_of_points * sizeof(*permutations));
	visited = calloc(number_of_points, sizeof(*visited));

	unsigned int points_visited = 0;

    // Makes sure to visit all the points, even if they are not anyone's neighbors (not recheable)
	for (unsigned int i = 0; i < points->num_points; ++i) {
		if (visited[i] == true)
			continue;

		enqueue(queue, i);

		while (is_queue_empty(queue) == false) {
			unsigned int new_index = dequeue(queue);

			permutations[points_visited] = new_index;
			++points_visited;

			visited[new_index] = true;

			size_t neighbours[K];
			double neighbours_distances[K];
			start_kdtree_knearest(tree, new_index, neighbours, neighbours_distances);

			for (unsigned int j = 0; j < K; ++j) {
				if (!visited[neighbours[j]]) {
					visited[neighbours[j]] = true;
					enqueue(queue, (unsigned int)neighbours[j]);
				}
			}
		}
	}

	if (!reserve_memory_points(new_points, points->num_points)) {
		destroyQueue(queue);
		free(permutations);
		free(visited);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	for (unsigned int i = 0; i < points_visited; ++i) { // points_visited == points->num_points
		add_point(new_points, i, points->x[permutations[points_visited-i-1]], points->y[permutations[points_visited-i-1]],
			  points->z[permutations[points_visited-i-1]]);
	}

	destroyQueue(queue);
	free(permutations);
	free(visited);
}

static int cmp_size_t(const void *a, const void *b) {
    size_t x = *(const size_t*)a;
    size_t y = *(const size_t*)b;

    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}


void reorder_bfs_sort_by_index(const KDTree *tree, const Points *points, Points *new_points)
{
	unsigned int number_of_points = (unsigned int)tree->pts->num_points;

	Queue *queue = createQueue(number_of_points);

	unsigned int *permutations;
	bool *visited;
	permutations = malloc(number_of_points * sizeof(*permutations));
	visited = calloc(number_of_points, sizeof(*visited));

	unsigned int points_visited = 0;

    // Makes sure to visit all the points, even if they are not anyone's neighbors (not recheable)
	for (unsigned int i = 0; i < points->num_points; ++i) {
		if (visited[i] == true)
			continue;

		enqueue(queue, i);

		while (is_queue_empty(queue) == false) {
			unsigned int new_index = dequeue(queue);

			permutations[points_visited] = new_index;
			++points_visited;

			visited[new_index] = true;

			size_t neighbours[K];
			double neighbours_distances[K];
			start_kdtree_knearest(tree, new_index, neighbours, neighbours_distances);

            qsort(neighbours, K, sizeof(size_t), cmp_size_t);

			for (unsigned int j = 0; j < K; ++j) {
				if (!visited[neighbours[j]]) {
					visited[neighbours[j]] = true;
					enqueue(queue, (unsigned int)neighbours[j]);
				}
			}
		}
	}

	if (!reserve_memory_points(new_points, points->num_points)) {
		destroyQueue(queue);
		free(permutations);
		free(visited);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	for (unsigned int i = 0; i < points_visited; ++i) { // points_visited == points->num_points
		add_point(new_points, i, points->x[permutations[i]], points->y[permutations[i]],
			  points->z[permutations[i]]);
	}

	destroyQueue(queue);
	free(permutations);
	free(visited);
}*/