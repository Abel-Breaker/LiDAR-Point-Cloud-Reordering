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

static size_t *degrees_original;
static size_t *degrees;
static size_t *indices;
static size_t counter;

int compare(const void *a, const void *b)
{
	const size_t ia = *(const size_t *)a;
	const size_t ib = *(const size_t *)b;

	if (degrees[ia] < degrees[ib])
		return -1;
	if (degrees[ia] > degrees[ib])
		return 1;
	return 0;
}

int compare_original(const void *a, const void *b)
{
	const size_t ia = *(const size_t *)a;
	const size_t ib = *(const size_t *)b;

	if (degrees_original[ia] < degrees_original[ib])
		return -1;
	if (degrees_original[ia] > degrees_original[ib])
		return 1;
	return 0;
}

static inline size_t get_point_index_lowest_degree(const bool *restrict visited, size_t num_points)
{
	for (; counter < num_points; ++counter) {
		if (!visited[indices[counter]]) {
			return indices[counter];
		}
	}
	return 0;
}

void reorder_cuthill_mckee(const Octree *octree, Points *new_points)
{
	// Reset counter between calls
	counter = 0;

	// Prepare some constant values
	const size_t num_points = octree->points->num_points;
	const double radius = get_args()->radius_search;

	// Create/Reserve auxiliar structures for reorder
	Queue *queue = create_queue(num_points); // Maximun posible size
	bool *visited = calloc(num_points, sizeof(*visited));
	size_t *permutations = malloc(sizeof(*permutations) * num_points);
	indices = malloc(sizeof(*indices) * num_points);
	degrees = malloc(sizeof(*degrees) * num_points);
	degrees_original = malloc(sizeof(*degrees_original) * num_points);

	// Check allocations
	if (!queue || !visited || !permutations || !degrees || !indices || !degrees_original) {
		destroy_queue(queue);
		free(visited);
		free(permutations);
		free(degrees);
		free(indices);
		free(degrees_original);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot allocate auxiliar structures for reorder RCM");
		return;
	}

	// Precalculate all points degrees
	#pragma omp parallel for schedule(static) // TODO: Test with schedule(dynamic)
	for (size_t i = 0; i < num_points; ++i) {
		degrees_original[i] = octree_radius_neighbor_count(octree, i, radius); // Mantain a copy of the array without reorder
		degrees[i] = degrees_original[i];
		indices[i] = i;
	}

	// Preorder all indices depending of the degrees
	qsort(indices, num_points, sizeof(*indices), compare);

	// Get the first point to visit and prepare for start the algorithm
	size_t min_grade_point_index = get_point_index_lowest_degree(visited, num_points);
	visited[min_grade_point_index] = true;
	enqueue(queue, min_grade_point_index);

	// Start the algorithm RCM to obtain permutations vector
	size_t points_visited = 0;
	while (points_visited < num_points) { // Make sure to visit all the points

		// If queue is empty, graph is disconnected so pick next unvisited node with lowest degree
		if (is_queue_empty(queue)) {
			min_grade_point_index = get_point_index_lowest_degree(visited, num_points);

			// Check if all nodes where visited
			if (min_grade_point_index == 0 && visited[min_grade_point_index] == true) {
				break;
			}
			
			visited[min_grade_point_index] = true;
			enqueue(queue, min_grade_point_index);
		}

		// Obtain next point
		size_t index = dequeue(queue);
		permutations[points_visited] = index;
		++points_visited;

		// Get points neighbors
		RadiusResultOctree result = {};
		octree_radius_search(octree, index, radius, &result);
		qsort(result.indices, result.count, sizeof(size_t), compare_original);

		// Process next point searching for new points to enqueue
		for (size_t i = 0; i < result.count; ++i) {
			if (visited[result.indices[i]] == false) {
				enqueue(queue, result.indices[i]);
				visited[result.indices[i]] = true;
			}
		}
		radius_result_destroy(&result);
	}

	// Reserves memory for new points
	if (!reserve_memory_points(new_points, num_points)) {
		destroy_queue(queue);
		free(permutations);
		free(visited);
		free(degrees);
		handle_error(ERROR_MALLOC, ERR_FATAL, "Cannot reserve memory for points");
		return;
	}

	// Save the points in the new order
	for (size_t i = 0; i < points_visited; ++i) {
		size_t idx = permutations[points_visited - 1 - i];
		add_point(new_points, i, octree->points->x[idx], octree->points->y[idx], octree->points->z[idx]);
	}

	// Free memory
	destroy_queue(queue);
	free(permutations);
	free(visited);
	free(degrees);
	free(indices);
}