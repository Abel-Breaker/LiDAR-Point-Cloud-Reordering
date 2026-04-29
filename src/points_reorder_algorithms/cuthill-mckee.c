#include "cuthill-mckee.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../types/neighborhood_matrix_mix/row.h"
#include "../utils/auxiliar_structures/queue.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdint.h> // for SIZE_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t *degrees;
static size_t *indices;


int compare(const void *a, const void *b) {
    size_t ia = *(const size_t *)a;
    size_t ib = *(const size_t *)b;

    if (degrees[ia] < degrees[ib]) return -1;
    if (degrees[ia] > degrees[ib]) return 1;
    return 0;
}

static inline void sort_matrix(struct matrix_t *matrix){

	#pragma omp parallel for schedule(static)
	for (size_t i = 0; i < matrix->points->num_points; ++i){
		qsort(matrix->rows[i]->indices, matrix->rows[i]->num_elements, sizeof(size_t), compare);
	}

}

static inline size_t get_point_index_lowest_degree(const bool *restrict visited, size_t num_points){
	static size_t counter = 0;
	for (; counter < num_points; ++counter) {
		if (!visited[indices[counter]]) {
			return indices[counter];
		}
	}
	return 0;
}

void reorder_cuthill_mckee(struct matrix_t *matrix, Points *new_points)
{
	const size_t num_points = matrix->points->num_points;
	bool *visited = calloc(num_points, sizeof(*visited));
	size_t *permutations = malloc(sizeof(*permutations) * num_points);
	Queue *queue = createQueue(num_points);
	size_t points_visited = 0;
	indices = malloc(matrix->points->num_points * sizeof(*indices));

	degrees = malloc(matrix->points->num_points * sizeof(*degrees));

	if (!visited || !permutations || !degrees || !indices) {
		free(visited);
		free(permutations);
		free(degrees);
		free(indices);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot allocate CK buffers");
		return;
	}

	// Precalculate all degrees
	for (size_t i = 0; i < matrix->points->num_points; ++i){
		degrees[i] = matrix->rows[i]->num_elements;
		indices[i] = i;
	}

	printf("Degrees calculated\n");

	sort_matrix(matrix);

	printf("Matrix ordered\n");

	qsort(indices, matrix->points->num_points, sizeof(size_t), compare);

	printf("Indices ordered\n");

	// Get point with lowest grade
	size_t min_grade_point_index = get_point_index_lowest_degree(visited, num_points);
	printf("Node with lowest grade: %zu with grade %zu\n", min_grade_point_index, degrees[min_grade_point_index]);

	visited[min_grade_point_index] = true;
	enqueue(queue, min_grade_point_index);

	//size_t max_num_elements = get_max_num_elements_row(matrix);

	size_t queue_empty_count = 0;

	while (points_visited < num_points) {

		// Si la cola está vacía, el grafo está desconectado:
		// buscar el siguiente nodo no visitado de menor grado
		if (is_queue_empty(queue)) {
			queue_empty_count++;
			min_grade_point_index = get_point_index_lowest_degree(visited, num_points);
			if(visited[min_grade_point_index] == true){ // Case where all nodes visited (return 0)
				break;
			}
			visited[min_grade_point_index] = true;
			enqueue(queue, min_grade_point_index);
		}

		size_t index = dequeue(queue);
		permutations[points_visited] = index;
		++points_visited;

		// Optimized access to the row directly (violating opaquing)
		const size_t *neighbors = get_neighbours_row(matrix->rows[index]);
		size_t num_elements = degrees[index];

		for (size_t i = 0; i < num_elements; ++i) {
			if (visited[neighbors[i]] == false) {
				enqueue(queue, neighbors[i]);
				visited[neighbors[i]] = true;
			}
		}

	}
	// Al final:
printf("Queue went empty %zu times\n", queue_empty_count);

	
	if (!reserve_memory_points(new_points, num_points)) {
		destroyQueue(queue);
		free(permutations);
		free(visited);
		free(degrees);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	for (size_t i = 0; i < points_visited; ++i) { // points_visited == points->num_points
		add_point(new_points, i, matrix->points->x[permutations[i]], matrix->points->y[permutations[i]],
			  matrix->points->z[permutations[i]]);
	}

	destroyQueue(queue);
	free(permutations);
	free(visited);
	free(degrees);
	free(indices);

}