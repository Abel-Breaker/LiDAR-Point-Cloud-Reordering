#define _POSIX_C_SOURCE 199309L
#include "neighborhood_bench.h"
#include "../src/points_sorted/points_sorted.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "../utils/timer.h"
#include "../utils/types.h"
#include <stdio.h>
#include <stdlib.h>

void neighborhoods_octree_radius_bench(const Octree *structure)
{
	timer_start();
#pragma omp parallel
	{
		RadiusResultOctree res = {};

#pragma omp for
		for (index_t i = 0; i < structure->points->num_points; ++i) {
			octree_radius_search(structure, i, get_args()->radius_search, &res);

			// Force use to avoid code elimination
			__asm__ volatile("" : : "r"(res.count) : "memory");
		}

		radius_result_destroy(&res);
	}
	timer_stop_and_print("Neighborhood radius");
}

void neighborhoods_tfg_bench(const Points_TFG *points)
{
	// Obtain radius and the search amplification factor as a function of the radius
	const data_t radius_search = get_args()->radius_search;
	const data_t radius_reorder = get_args()->radius_reorder;
	const index_t factor = (index_t)ceil(radius_search / radius_reorder); // Dangerous

	// Test neighborhood
	timer_start();
#pragma omp parallel
	{
		RadiusResult res = {};
		reserves_memory_radius_result(&res, points->max_bandwith * factor);

#pragma omp for
		for (index_t i = 0; i < points->points->num_points; ++i) {

			tfg_radius_search(points, i, &res);

			// Force use to avoid code elimination
			__asm__ volatile("" : : "r"(res.count) : "memory");
		}
		destroy_radius_result(&res);
	}
	timer_stop_and_print("Neighborhood radius");
}