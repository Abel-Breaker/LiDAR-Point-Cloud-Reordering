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

static void sort_neighbors(size_t *idx, size_t *grade, size_t k)
{
	for (size_t i = 0; i < k - 1; i++) {
		for (size_t j = i + 1; j < k; j++) {
			if (grade[j] < grade[i]) {
				// swap grade
				size_t dtmp = grade[i];
				grade[i] = grade[j];
				grade[j] = dtmp;

				// swap idx
				size_t itmp = idx[i];
				idx[i] = idx[j];
				idx[j] = itmp;
			}
		}
	}
}

static inline size_t get_point_index_lowest_degree(const size_t *restrict degrees, const bool *restrict visited, size_t num_points){
	size_t min_grade = SIZE_MAX;
	size_t min_grade_point_index = 0;
	for (size_t i = 0; i < num_points; ++i) {
		if (!visited[i] && degrees[i] < min_grade) {
			min_grade = degrees[i];
			min_grade_point_index = i;
		}
	}
	return min_grade_point_index;
}

void reorder_cuthill_mckee(const struct matrix_t *matrix, Points *new_points)
{
	const size_t num_points = matrix->points->num_points;
	bool *visited = calloc(num_points, sizeof(*visited));
	size_t *permutations = malloc(sizeof(*permutations) * num_points);
	Queue *queue = createQueue(num_points);
	size_t points_visited = 0;

	size_t *degrees = malloc(num_points * sizeof(*degrees));
	for (size_t i = 0; i < num_points; ++i){
		degrees[i] = matrix->rows[i]->num_elements;
	}

	// Get point with lowest grade
	size_t min_grade_point_index = get_point_index_lowest_degree(degrees, visited, num_points);
	printf("Node with lowest grade: %zu with grade %zu\n", min_grade_point_index, degrees[min_grade_point_index]);

	visited[min_grade_point_index] = true;
	enqueue(queue, min_grade_point_index);

	size_t max_num_elements = get_max_num_elements_row(matrix);
	size_t *neighbors_cpy = malloc(sizeof(*neighbors_cpy) * max_num_elements);
	size_t *neighbors_grade = malloc(sizeof(*neighbors_grade) * max_num_elements);

	while (points_visited < num_points) {

		// Si la cola está vacía, el grafo está desconectado:
		// buscar el siguiente nodo no visitado de menor grado
		if (is_queue_empty(queue)) {
			min_grade_point_index = get_point_index_lowest_degree(degrees, visited, num_points);
			visited[min_grade_point_index] = true;
			enqueue(queue, min_grade_point_index);
		}

		size_t index = dequeue(queue);
		permutations[points_visited] = index;
		++points_visited;

		// Optimized access to the row directly (violating opaquing)
		const size_t *neighbors = get_neighbours_row(matrix->rows[index]);
		size_t num_elements = degrees[index];

		memcpy(neighbors_cpy, neighbors, sizeof(*neighbors_cpy) * num_elements);
		for (size_t i = 0; i < num_elements; ++i) {
			neighbors_grade[i] = degrees[neighbors_cpy[i]];
		}

		sort_neighbors(neighbors_cpy, neighbors_grade, num_elements);

		for (size_t i = 0; i < num_elements; ++i) {
			if (visited[neighbors_cpy[i]] == false) {
				enqueue(queue, neighbors_cpy[i]);
				visited[neighbors_cpy[i]] = true;
			}
		}
	}
	free(neighbors_cpy);
	free(neighbors_grade);
	free(degrees);

	if (!reserve_memory_points(new_points, num_points)) {
		destroyQueue(queue);
		free(permutations);
		free(visited);
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
}