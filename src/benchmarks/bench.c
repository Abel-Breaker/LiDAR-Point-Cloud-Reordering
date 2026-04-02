#include "bench.h"
#define _POSIX_C_SOURCE 199309L
#include "../neighborhood_algorithms/knn_kd_tree.h"
#include "../neighborhood_algorithms/search_octree.h"
#include "../points_structures/kd_tree.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "neighborhood_bench.h"
#include "points_structures_bench.h"
#include <stdlib.h>
#include <time.h>

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