#include "test.h"
#include "../src/octree/octree.h"
#include "../src/points_sorted/opt/points_sorted_opt.h"
#include "../src/points_sorted/points_sorted.h"
#include "../src/reorder/cuthill-mckee.h"
#include "neighborhood_test.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static void test_points_for_octree(const Points *points)
{
	// Octree creation
	Octree octree = {};
	create_octree(&octree, points);

	// Checks
	check_neighborhoods_octree_radius(&octree);

	destroy_octree(&octree);
}

static void test_points_for_tfg(const Points *points)
{

	// Create necesary structures
	Octree octree = {};
	create_octree(&octree, points);

	{
		printf("\n\033[1mTFG\033[0m\n");
		Points_TFG points_sorted = {};
		size_t *permutations = malloc(sizeof(*permutations) * points->num_points);
		reorder_cuthill_mckee(&octree, permutations);
		build_sorted_points(&points_sorted, &octree, permutations);
		check_neighborhoods_tfg(&points_sorted);
		//check_neighborhoods_tfg_opt(&points_sorted);
		destroy_points_sorted(&points_sorted);
		free(permutations);
	}
}


void test(const Points *points)
{
	srand((unsigned)time(NULL));

	const char *pink = "\033[1;95m";
	const char *reset = "\033[0m";

	printf("\n");

	printf("%sTesting Octree...\n%s", pink, reset);
	//test_points_for_octree(points);

	printf("%sTesting TFG..\n%s", pink, reset);
	test_points_for_tfg(points);

	
}