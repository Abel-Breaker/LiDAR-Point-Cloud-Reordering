#include "test.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix.h"
#include "../types/neighborhood_matrix_raw.h"
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

static void test_points_for_matrix(const Points *points)
{
	// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		neighborhood_matrix matrix = nullptr;
		create_neighborhood_matrix(&matrix, &tree);

		check_neighborhoods_matrix(matrix, points);

		destroy_neighborhood_matrix(matrix, points->num_points);
		destroy_kd_tree(&tree);
}

static void test_points_for_matrix_raw(const Points *points)
{
	// Create new kd_tree
		KDTree tree = {};
		create_kd_tree(&tree, points);

		neighborhood_matrix_raw matrix = nullptr;
		create_neighborhood_matrix_raw(&matrix, &tree);

		check_neighborhoods_matrix_raw(matrix, points);

		destroy_neighborhood_matrix_raw(matrix, points->num_points);
		destroy_kd_tree(&tree);
}

void test(const Points *points)
{
	printf("Testing KD-Tree\n");
	test_points_for_kd_tree(points);
	printf("Testing KD-Tree Prune\n");
	test_points_for_kd_tree_prune(points);
	printf("Testing Octree\n");
	test_points_for_octree(points);
	printf("Testing Matrix\n");
	test_points_for_matrix(points);
	printf("Testing Matrix Raw\n");
	test_points_for_matrix_raw(points);
}