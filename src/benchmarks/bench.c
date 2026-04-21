#define _POSIX_C_SOURCE 199309L
#include "bench.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../neighborhood_algorithms/knn/kd_tree_prune.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix_mix.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "neighborhood_bench.h"
#include "points_structures_bench.h"
#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bench(const Points *points)
{
	// Init PAPI
	int EventSet = PAPI_NULL;
	long long values[2];
	int events[2] = {PAPI_L1_DCM, PAPI_L2_DCM};

	int ret;

	// Init library
	ret = PAPI_library_init(PAPI_VER_CURRENT);
	if (ret != PAPI_VER_CURRENT && ret < 0) {
		fprintf(stderr, "PAPI init error\n");
	}

	// Create event set
	ret = PAPI_create_eventset(&EventSet);
	if (ret != PAPI_OK) {
		fprintf(stderr, "PAPI_create_eventset error\n");
	}

	// Add events one by one (MUCHO más compatible)
	ret = PAPI_add_event(EventSet, PAPI_L1_DCM);
	if (ret != PAPI_OK)
		fprintf(stderr, "L1 event not supported\n");

	ret = PAPI_add_event(EventSet, PAPI_L2_DCM);
	if (ret != PAPI_OK)
		fprintf(stderr, "L2 event not supported\n");

	// Create new kd_tree
	KDTree tree = {};
	create_kd_tree(&tree, points);

	// KD-TREE (creation + knn)
	{
		printf("\n\033[1mKD-TREE\033[0m\n");
		// Benchmark KD-Tree creation
		kd_tree_benchmark(points);

		// Benchmark neighborhoods
		PAPI_start(EventSet);
		neighborhoods_kd_tree_knn_bench(&tree);
		PAPI_stop(EventSet, values);
		printf("\tL1 cache misses: %lld\n", values[0]);
		printf("\tL2 cache misses: %lld\n", values[1]);
	}

	// KD-TREE Prune (creation + knn)
	{
		printf("\n\033[1mKD-TREE Prune\033[0m\n");
		// Create new kd_tree
		KDTreePrune tree_prune = {};
		create_kd_tree_prune(&tree_prune, points);

		struct timespec start, end;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		set_upper_bound_distance(&tree_prune);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("\tSet upper bound distance: %.6f s\n",
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000);

		// Benchmark neighborhoods
		neighborhoods_kdtree_prune_knn_bench(&tree_prune);

		destroy_kd_tree_prune(&tree_prune);
	}

	// OCTREE (creation + knn + radius)
	{
		printf("\n\033[1mOCTREE\033[0m\n");
		// Benchmark Octree creation
		octree_benchmark(points);

		// Octree creation and check
		Octree octree = {};
		create_octree(&octree, points);

		// Benchmark neighborhoods
		neighborhoods_octree_knn_bench(&octree);
		neighborhoods_octree_radius_bench(&octree);

		destroy_octree(&octree);
	}

	{
		printf("\n\033[1mMATRIX MIX\033[0m\n");

		struct timespec start, end;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		matrix_mix matrix = {};
		create_neighbourhood_matrix_mix(&matrix, &tree);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("\tCreate matrix: %.6f s\n",
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000);

		neighborhoods_matrix_mix_bench(&matrix);

		print_matrix_mix_stats(&matrix);

		destroy_neighbourhood_matrix_mix(&matrix);
	}

	destroy_kd_tree(&tree);
}