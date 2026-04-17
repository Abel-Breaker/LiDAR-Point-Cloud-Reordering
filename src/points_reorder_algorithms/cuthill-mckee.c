#include "cuthill-mckee.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../utils/auxiliar_structures/queue.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdio.h>
#include <stdlib.h>
/*
void reorder_cuthill_mckee(const Octree *tree, const Points *points, Points *new_points){
	unsigned int number_of_points = (unsigned int)tree->pts->num_points;

	Queue *queue = createQueue(number_of_points);

	RadiusResult *res = malloc(sizeof(*res) * number_of_points);
	size_t min_grade_index=0;
	size_t min_grade=INFINITY;

	// keep 
	for(size_t i=0; i<number_of_points; ++i){
		octree_radius_search(tree, i, get_args()->radius_search, res+i);
		if(res[i].count < min_grade){
			min_grade = res[i].count;
			min_grade_index = i;
		}
	}

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
			start_octree_knearest(tree, new_index, neighbours, neighbours_distances);

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
}*/