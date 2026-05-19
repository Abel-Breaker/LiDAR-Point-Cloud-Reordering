#define _POSIX_C_SOURCE 199309L
#include "neighborhood_bench.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../neighborhood_algorithms/radius_search/tfg_idea.h"
#include "../neighborhood_algorithms/radius_search/tfg_idea_opt.h"
#include "../points_structures/octree.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void neighborhoods_matrix_bench(const struct matrix_t *matrix)
{
	struct timespec start, end;
	double total = 0;

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	#pragma omp parallel for
	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		RadiusResult res = {};
		get_neighbours_matrix(matrix, i, &res);

		// Force use to avoid code elimination
		__asm__ volatile("" : : "r"(res.count) : "memory");
		destroy_radius_result(&res);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("\tNeighborhood radius: %.6f s\n", total);
}

void neighborhoods_octree_radius_bench(const Octree *structure)
{
	struct timespec start, end;
	double total = 0;

	volatile double sink_dist = 0;

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
#pragma omp parallel for
	for (size_t i = 0; i < structure->points->num_points; ++i) {
		RadiusResultOctree res = {};
		octree_radius_search(structure, i, get_args()->radius_search, &res);
		
		// Force use to avoid code elimination
		__asm__ volatile("" : : "r"(res.count) : "memory");
		__asm__ volatile("" : : "r"(res.indices) : "memory");
		
		radius_result_destroy(&res);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("\tNeighborhood radius: %.6f s\n", total);

	(void)sink_dist;
}

void neighborhoods_tfg_bench(const struct matrix_t *matrix)
{
	struct timespec start, end;
	double total = 0;

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	#pragma omp parallel for
	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		RadiusResult res = {};
		tfg_radius_search(matrix->points, i, matrix->bandwith_left[get_block_index(i, matrix->points->num_points)], matrix->bandwith_right[get_block_index(i, matrix->points->num_points)], &res);

		// Force use to avoid code elimination
		__asm__ volatile("" : : "r"(res.count) : "memory");
		destroy_radius_result(&res);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("\tNeighborhood radius: %.6f s\n", total);

	tfg_print_timing_stats();
}

void neighborhoods_tfg_opt_bench(const struct matrix_t *matrix)
{
	struct timespec start, end;
	double total = 0;

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	#pragma omp parallel for
	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		RadiusResult res = {};
		tfg_radius_search_opt(matrix->points, i, matrix->bandwith_left[get_block_index(i, matrix->points->num_points)], matrix->bandwith_right[get_block_index(i, matrix->points->num_points)], &res);

		// Force use to avoid code elimination
		__asm__ volatile("" : : "r"(res.count) : "memory");
		__asm__ volatile("" : : "r"(res.indices) : "memory");
		
		destroy_radius_result(&res);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("\n\tNeighborhood radius opt: %.6f s\n", total);

	tfg_print_timing_stats_opt();
}