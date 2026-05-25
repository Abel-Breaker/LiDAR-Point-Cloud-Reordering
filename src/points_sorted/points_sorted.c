#include "points_sorted.h"
#include "../../utils/parse_args.h"
#include "../octree/octree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

size_t get_block_index(size_t i, size_t num_points)
{
	const size_t number_of_blocks = get_args()->number_of_blocks;

	if (num_points == 0)
		return 0;
	size_t block_size = (num_points + number_of_blocks - 1) / number_of_blocks; // ceil
	size_t block = i / block_size;
	return (block >= number_of_blocks) ? number_of_blocks - 1 : block;
}

static bool reserve_memory_points_sorted(Points_TFG *points, size_t number_of_points)
{
	points->points = malloc(sizeof(*(points->points)));

	if (!reserve_memory_points(points->points, number_of_points)) {
		return false;
	}

	points->bandwith_left = calloc(get_args()->number_of_blocks, sizeof(size_t));
	points->bandwith_right = calloc(get_args()->number_of_blocks, sizeof(size_t));

	return true;
}

static void print_points_sorted_stats(const size_t *bandwith_left, const size_t *bandwith_right, size_t num_points)
{
	size_t max_left = 0;
	size_t max_right = 0;
	size_t max_total = 0;
	double avg_total = 0.0;

	for (size_t i = 0; i < num_points; ++i) {
		size_t total = bandwith_left[i] + bandwith_right[i];

		if (bandwith_left[i] > max_left)
			max_left = bandwith_left[i];
		if (bandwith_right[i] > max_right)
			max_right = bandwith_right[i];
		if (total > max_total)
			max_total = total;

		avg_total += (double)total;
	}

	avg_total /= (double)num_points;

	printf("\nMax bandwidth   : %zu\n", max_total);
	printf("Max left        : %zu\n", max_left);
	printf("Max right       : %zu\n", max_right);
	printf("Average total   : %.2f\n", avg_total);
}

void build_sorted_points(Points_TFG *points, const Points *old_points, const Solution *solution)
{
	const size_t num_points = old_points->num_points;

	reserve_memory_points_sorted(points, num_points);

	for (size_t i = 0; i < num_points; ++i) { // TODO: parallelize?
		size_t idx = solution->permutations[num_points - 1 - i];
		add_point(points->points, i, old_points->x[idx], old_points->y[idx], old_points->z[idx]);
	}

	for (size_t i = 0; i < num_points; ++i) {
		size_t block = get_block_index(i, num_points);
		size_t rcm_i = num_points - 1 - i;

		// left y right se intercambian al invertir
		if (solution->bandwith_right[rcm_i] > points->bandwith_left[block])
			points->bandwith_left[block] = solution->bandwith_right[rcm_i];
		if (solution->bandwith_left[rcm_i] > points->bandwith_right[block])
			points->bandwith_right[block] = solution->bandwith_left[rcm_i];
	}

	print_points_sorted_stats(solution->bandwith_left, solution->bandwith_right, num_points);
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