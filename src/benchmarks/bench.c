#define _POSIX_C_SOURCE 199309L
#include "bench.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../neighborhood_algorithms/knn/kd_tree_prune.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "neighborhood_bench.h"
#include "points_structures_bench.h"
#include "../types/neighborhood_matrix.h"
#include "../types/neighborhood_matrix_raw.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bench(const Points *points)
{

	// KD-TREE (creation + knn)
	{
		printf("\n\033[1mKD-TREE\033[0m\n");
		// Benchmark KD-Tree creation
		kd_tree_benchmark(points);

		// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		// Benchmark neighborhoods
		neighborhoods_kd_tree_knn_bench(&tree);

		destroy_kd_tree(&tree);
	}

	{
		printf("\n\033[1mMATRIX\033[0m\n");

		// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		struct timespec start, end;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		neighborhood_matrix matrix = nullptr;
		create_neighborhood_matrix(&matrix, &tree);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("\tCreate matrix: %.6f s\n",
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000);

		neighborhoods_matrix_bench(matrix, points);

		print_matrix_stats(matrix, points->num_points);

		destroy_neighborhood_matrix(matrix, points->num_points);
		destroy_kd_tree(&tree);
	}

	{
		printf("\n\033[1mMATRIX RAW\033[0m\n");

		// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		struct timespec start, end;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		neighborhood_matrix_raw matrix = nullptr;
		create_neighborhood_matrix_raw(&matrix, &tree);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("\tCreate matrix: %.6f s\n",
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000);

		neighborhoods_matrix_bench_raw(matrix, points);

		print_matrix_stats_raw(matrix, points->num_points);

		destroy_neighborhood_matrix_raw(matrix, points->num_points);
		destroy_kd_tree(&tree);
	}

	// KD-TREE (creation + knn)
	{
		printf("\n\033[1mKD-TREE Prune\033[0m\n");
		// Create new kd_tree
		KDTreePrune tree = {};
		create_kd_tree_prune(&tree, points);

		struct timespec start, end;
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		set_upper_bound_distance(&tree);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		printf("\tSet upper bound distance: %.6f s\n",
		       (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000);

		// Benchmark neighborhoods
		neighborhoods_kdtree_prune_knn_bench(&tree);

		destroy_kd_tree_prune(&tree);
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
}