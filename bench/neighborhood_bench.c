#define _POSIX_C_SOURCE 199309L
#include "neighborhood_bench.h"
#include "../src/points_sorted/opt/points_sorted_opt.h"
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
	// Test neighborhood
	timer_start();
#pragma omp parallel for
	for (index_t i = 0; i < points->points->num_points; ++i) {
		RadiusResult res = {};
		tfg_radius_search(points, i, &res);

		// Force use to avoid code elimination
		__asm__ volatile("" : : "r"(res.count) : "memory");
		destroy_radius_result(&res);
	}
	timer_stop_and_print("Neighborhood radius");
}

/*
void neighborhoods_tfg_opt_bench(const Points_TFG_opt *points)
{
	struct timespec start, end;
	double total = 0;

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	#pragma omp parallel for
	for (index_t i = 0; i < points->points->num_points; ++i) {
		RadiusResult res = {};
		tfg_radius_search_opt(points, i, &res);

		// Force use to avoid code elimination
		__asm__ volatile("" : : "r"(res.count) : "memory");
		__asm__ volatile("" : : "r"(res.indices) : "memory");

		destroy_radius_result(&res);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("\n\tNeighborhood radius opt: %.6f s\n", total);
}*/