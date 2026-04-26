#include "test.h"
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix.h"
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
	check_neighborhoods_octree_knn(&octree);
	check_neighborhoods_octree_radius(&octree);

	destroy_octree(&octree);
}

static void test_points_for_matrix_mix(const Points *points)
{
	// Octree creation
	Octree octree = {};
	create_octree(&octree, points);

	struct matrix_t matrix = {};
	create_neighbourhood_matrix(&matrix, &octree);

	check_neighborhoods_matrix_mix(&matrix);

	destroy_neighbourhood_matrix(&matrix);
	destroy_octree(&octree);
}

void test(const Points *points)
{
	srand((unsigned)time(NULL));

	const char *pink = "\033[1;95m";
	const char *reset = "\033[0m";

	printf("\n");

	printf("%sTesting Octree...\n%s", pink, reset);
	test_points_for_octree(points);

	printf("%sTesting Matrix Mix..\n%s", pink, reset);
	test_points_for_matrix_mix(points);
}