#include "test.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"
#include "neighborhood_test.h"

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

void test(const Points *points)
{
	test_points_for_kd_tree(points);
	test_points_for_kd_tree_prune(points);
	test_points_for_octree(points);
}