#define _POSIX_C_SOURCE 199309L
#include "bench.h"
#include "../src/octree/octree.h"
#include "../src/points_sorted/points_sorted.h"
#include "../src/reorder/cuthill-mckee.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "neighborhood_bench.h"
#include "points_structures_bench.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bench(const Points *points)
{

	// Create necesary structures
	Octree octree = {};
	create_octree(&octree, points);

	{
		printf("\n\033[1mOCTREE\033[0m\n");
		neighborhoods_octree_radius_bench(&octree);
	}

	{
		printf("\n\033[1mTFG\033[0m\n");
		Points_TFG points_sorted = {};

		Solution *sol = reorder_cuthill_mckee(&octree);
		build_sorted_points(&points_sorted, octree.points, sol);
		destroy_solution(sol);

		neighborhoods_tfg_bench(&points_sorted);
		
		destroy_points_sorted(&points_sorted);
	}

	destroy_octree(&octree);
}