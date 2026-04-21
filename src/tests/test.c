#include "test.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix_mix.h"
#include "neighborhood_test.h"
#include <stdio.h>

static void test_points_for_kd_tree(const Points *points)
{
	// Create kdtree
	KDTree tree = {};
	create_kd_tree(&tree, points);

	// Checks
	check_kd_tree(&tree);
	check_neighborhoods_kd_tree(&tree);

	destroy_kd_tree(&tree);
}

static void test_points_for_kd_tree_prune(const Points *points)
{
	// Create kdtree
	KDTreePrune tree = {};
	create_kd_tree_prune(&tree, points);

	// Checks
	check_kd_tree_prune(&tree);
	check_neighborhoods_kd_tree_prune(&tree);

	destroy_kd_tree_prune(&tree);
}

static void test_points_for_octree(const Points *points)
{
	// Octree creation
	Octree octree = {};
	create_octree(&octree, points);

	// Checks
	check_neighborhoods_octree_knn(&octree);
	check_neighborhoods_octree_radius(&octree);

	destroy_octree(&octree);
}

static void test_points_for_matrix_mix(const Points *points)
{
	// Create new kd_tree
	KDTree tree = {};
	create_kd_tree(&tree, points);

	matrix_mix matrix = {};
	create_neighbourhood_matrix_mix(&matrix, &tree);

	check_neighborhoods_matrix_mix(&matrix);

	destroy_neighbourhood_matrix_mix(&matrix);
	destroy_kd_tree(&tree);
}

void test(const Points *points)
{
	const char *pink = "\033[1;95m";
	const char *reset = "\033[0m";

	printf("\n");
	printf("%sTesting KD-Tree...\n%s", pink, reset);
	test_points_for_kd_tree(points);

	printf("%sTesting KD-Tree Prune...\n%s", pink, reset);
	test_points_for_kd_tree_prune(points);

	printf("%sTesting Octree...\n%s", pink, reset);
	test_points_for_octree(points);

	printf("%sTesting Matrix Mix..\n%s", pink, reset);
	test_points_for_matrix_mix(points);
}