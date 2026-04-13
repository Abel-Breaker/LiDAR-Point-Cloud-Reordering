#include "bench.h"
#include "../neighborhood_algorithms/knn_kd_tree.h"
#include "../neighborhood_algorithms/knn_kd_tree_prune.h"
#include "../neighborhood_algorithms/search_octree.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "neighborhood_bench.h"
#include "points_structures_bench.h"
#include <stdlib.h>
#include <stdio.h>

void bench(const Points *points)
{

	// KD-TREE (creation + knn)
	{
		// Benchmark KD-Tree creation
		kd_tree_benchmark(points);

		// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		// Benchmark neighborhoods
		neighborhoods_kd_tree_knn_bench(&tree);

		destroy_kd_tree(&tree);
	}

	// KD-TREE (creation + knn)
	{
		printf("Prune ");
		// Create new kd_tree
		KDTreePrune tree = {};
		create_kd_tree_prune(&tree, points);
		set_upper_bound_distance(&tree);

		// Benchmark neighborhoods
		neighborhoods_kdtree_prune_knn_bench(&tree);

		destroy_kd_tree_prune(&tree);
	}

	// OCTREE (creation + knn + radius)
	{
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


#define CACHE_FLUSH_SIZE (50 * 1024 * 1024) // 50 MB, ajustar según CPU

char *flush_buf;

static void init_flush() {
    flush_buf = malloc(CACHE_FLUSH_SIZE);
    for (size_t i = 0; i < CACHE_FLUSH_SIZE; i++) {
        flush_buf[i] = (char)i;
    }
}

static void flush_cache() {
    for (size_t i = 0; i < CACHE_FLUSH_SIZE; i++) {
        flush_buf[i] += 1;
    }
}

void cold_bench(const Points *points)
{

	// KD-TREE (creation + knn)
	{
		// Benchmark KD-Tree creation
		kd_tree_benchmark(points);

		// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		init_flush();
		flush_cache();

		// Benchmark neighborhoods
		neighborhoods_kd_tree_knn_bench(&tree);

		destroy_kd_tree(&tree);
	}

	// OCTREE (creation + knn + radius)
	{
		// Benchmark Octree creation
		octree_benchmark(points);

		// Octree creation and check
		Octree octree = {};
		create_octree(&octree, points);

		// Benchmark neighborhoods
		init_flush();
		flush_cache();
		neighborhoods_octree_knn_bench(&octree);
		init_flush();
		flush_cache();
		neighborhoods_octree_radius_bench(&octree);

		destroy_octree(&octree);
	}

	//free(flush_buf);
}