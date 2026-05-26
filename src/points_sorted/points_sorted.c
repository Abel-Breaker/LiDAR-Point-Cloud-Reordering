#include "points_sorted.h"
#include "../../utils/parse_args.h"
#include "../octree/octree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

index_t get_block_index(index_t i, index_t num_points)
{
	const index_t number_of_blocks = get_args()->number_of_blocks;

	if (num_points == 0)
		return 0;
	index_t block_size = (num_points + number_of_blocks - 1) / number_of_blocks; // ceil
	index_t block = i / block_size;
	return (block >= number_of_blocks) ? number_of_blocks - 1 : block;
}

static bool reserve_memory_points_sorted(Points_TFG *points, index_t number_of_points)
{
	points->points = malloc(sizeof(*(points->points)));

	if (!reserve_memory_points(points->points, number_of_points)) {
		return false;
	}

	points->bandwith_left = calloc(get_args()->number_of_blocks, sizeof(*(points->bandwith_left)));
	points->bandwith_right = calloc(get_args()->number_of_blocks, sizeof(*(points->bandwith_right)));
	points->max_bandwith = 0;

	return true;
}

void build_sorted_points(Points_TFG *points, const Points *old_points, const Solution *solution)
{
	const index_t num_points = old_points->num_points;

	reserve_memory_points_sorted(points, num_points);

	for (index_t i = 0; i < num_points; ++i) { // TODO: parallelize?
		index_t idx = solution->permutations[num_points - 1 - i];
		add_point(points->points, i, old_points->x[idx], old_points->y[idx], old_points->z[idx]);
	}

	for (index_t i = 0; i < num_points; ++i) {
		index_t block = get_block_index(i, num_points);
		index_t rcm_i = num_points - 1 - i;

		// left y right se intercambian al invertir
		if (solution->bandwith_right[rcm_i] > points->bandwith_left[block])
			points->bandwith_left[block] = solution->bandwith_right[rcm_i];
		if (solution->bandwith_left[rcm_i] > points->bandwith_right[block])
			points->bandwith_right[block] = solution->bandwith_left[rcm_i];
		
		if((solution->bandwith_right[rcm_i] + solution->bandwith_left[rcm_i]) > points->max_bandwith){
			points->max_bandwith = solution->bandwith_right[rcm_i] + solution->bandwith_left[rcm_i];
		}
	}
}

void destroy_points_sorted(Points_TFG *points)
{
	if (!points)
		return;

	destroy_points(points->points);
	free(points->points);
	free(points->bandwith_left);
	free(points->bandwith_right);

	points->bandwith_left = points->bandwith_right = NULL;
}