#define _POSIX_C_SOURCE 199309L
#include "points_structures_bench.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/octree.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define ITER_WARM_UP 0
#define ITER 1

void kd_tree_benchmark(const Points *points)
{
	struct timespec start, end;
	double total=0;

	// Create and check new kd_tree
	KDTree tree = {};

	// WARM UP
	for (int i = 0; i < ITER_WARM_UP; ++i) {
		create_kd_tree(&tree, points);
		destroy_kd_tree(&tree);
	}

	for (int i = 0; i < ITER; ++i) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		create_kd_tree(&tree, points);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

		destroy_kd_tree(&tree);
	}

	printf("\tCreation: %.6f s\n", total / ITER);
}

void octree_benchmark(const Points *points)
{
	struct timespec start, end;
	double total=0;

	// Create and check new kd_tree
	Octree octree = {};

	// WARM UP
	for (int i = 0; i < ITER_WARM_UP; ++i) {
		create_octree(&octree, points);
		destroy_octree(&octree);
	}

	for (int i = 0; i < ITER; ++i) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		create_octree(&octree, points);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

		destroy_octree(&octree);
	}

	printf("\tCreation: %.6f s\n", total / ITER);
}