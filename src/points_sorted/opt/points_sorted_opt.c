#include "points_sorted_opt.h"
#include "../../../utils/parse_args.h"
#include "../../octree/octree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

size_t get_block_index_opt(size_t i, size_t num_points)
{
	const size_t number_of_blocks = get_args()->number_of_blocks;

	if (num_points == 0)
		return 0;
	size_t block_size = (num_points + number_of_blocks - 1) / number_of_blocks; // ceil
	size_t block = i / block_size;
	return (block >= number_of_blocks) ? number_of_blocks - 1 : block;
}

static bool reserve_memory_points_sorted(Points_TFG_opt *points, size_t number_of_points)
{

	if (!reserve_memory_points(points->points, number_of_points)) {
		return false;
	}

	points->bandwith_left = calloc(get_args()->number_of_blocks, sizeof(size_t));
	points->bandwith_right = calloc(get_args()->number_of_blocks, sizeof(size_t));

	points->stats.avg_bandwith = 0;
	points->stats.avg_neighbours = 0;
	points->stats.max_bandwith = 0;
	points->stats.max_neighbours = 0;
	points->stats.num_neighbours = 0;

	return true;
}

static void build_sorted_points_bounds_and_stats(Points_TFG_opt *points, const Octree *octree)
{
    const size_t num_points = octree->points->num_points;
    const double radius = get_args()->radius_search;

    size_t *tmp_bandwith_left  = malloc(num_points * sizeof(*tmp_bandwith_left));
    size_t *tmp_bandwith_right = malloc(num_points * sizeof(*tmp_bandwith_right));
    size_t *tmp_neighbours     = malloc(num_points * sizeof(*tmp_neighbours));

    size_t max_neighbours = 0;

    #pragma omp parallel
    {
        RadiusResultOctree res = {};

        #pragma omp for reduction(max : max_neighbours)
        for (size_t i = 0; i < num_points; ++i) {
            octree_radius_search(octree, i, radius, &res);

            // El punto i mismo puede estar incluido en res; se excluye del conteo
            size_t neighbours = res.count;
            size_t min = i;
            size_t max = i;

            for (size_t j = 0; j < res.count; ++j) {
                if (res.indices[j] < min) min = res.indices[j];
                if (res.indices[j] > max) max = res.indices[j];
            }

            tmp_bandwith_left[i]  = i - min;
            tmp_bandwith_right[i] = max - i;
            tmp_neighbours[i]     = neighbours;

            if (neighbours > max_neighbours) max_neighbours = neighbours;
        }

        radius_result_destroy(&res);
    }

    // Build bounds y stats
    size_t total_neighbours = 0;
    size_t total_bandwith   = 0;
    size_t max_bandwith     = 0;

    for (size_t i = 0; i < num_points; ++i) {
        // Bounds por bloque
        size_t block = get_block_index_opt(i, num_points);
        if (tmp_bandwith_left[i]  > points->bandwith_left[block])
            points->bandwith_left[block]  = tmp_bandwith_left[i];
        if (tmp_bandwith_right[i] > points->bandwith_right[block])
            points->bandwith_right[block] = tmp_bandwith_right[i];

        // Acumuladores para stats
        total_neighbours += tmp_neighbours[i];

        size_t bw = tmp_bandwith_left[i] + tmp_bandwith_right[i];
        if (bw > max_bandwith) max_bandwith = bw;
        total_bandwith += bw;
    }

    // Rellenar stats
    points->stats.num_neighbours  = total_neighbours;
    points->stats.max_neighbours  = max_neighbours;
    points->stats.avg_neighbours  = (float)total_neighbours / (float)num_points;
    points->stats.max_bandwith    = max_bandwith;
    points->stats.avg_bandwith    = (float)total_bandwith   / (float)num_points;

    free(tmp_bandwith_left);
    free(tmp_bandwith_right);
    free(tmp_neighbours);
}

void build_sorted_points_opt(Points_TFG_opt *points, const Octree *octree, const size_t *permutations)
{
	const size_t num_points = octree->points->num_points;

    printf("HOLA\n");
	reserve_memory_points_sorted(points, num_points);
    printf("HOLA 2\n");

	for (size_t i = 0; i < num_points; ++i) { // TODO: parallelize?
		size_t idx = permutations[num_points - 1 - i];
		add_point(points->points, i, octree->points->x[idx], octree->points->y[idx], octree->points->z[idx]);
	}

	build_sorted_points_bounds_and_stats(points, octree);
}

void destroy_points_opt(Points_TFG_opt *points)
{
	if (!points)
		return;

	destroy_points(points->points);
	free(points->bandwith_left);
	free(points->bandwith_right);

	points->bandwith_left = points->bandwith_right = NULL;
}