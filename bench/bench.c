#include "bench.h"
#include "../src/octree/octree.h"
#include "../src/points_sorted/points_sorted.h"
#include "../src/reorder/cuthill-mckee.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include "../utils/timer.h"
#include "../utils/terminal_formating.h"
#include "neighborhood_bench.h"
#include <stdio.h>
#include <stdlib.h>

void bench(const Points *points)
{

	// Create base structure
	timer_start();
	Octree octree = {};
	create_octree(&octree, points);
	timer_stop_and_print("Build octree");

	{
		printf(BOLD_BLUE "\nOCTREE\n" COLOR_RESET);
		neighborhoods_octree_radius_bench(&octree);
	}

	{
		printf(BOLD_BLUE "\nTFG\n" COLOR_RESET);

		timer_start();
		Solution *sol = reorder_cuthill_mckee(&octree);
		timer_stop_and_print("Reorder");

		timer_start();
		Points_sorted points_sorted = {};
		build_sorted_points(&points_sorted, octree.points, sol);
		timer_stop_and_print("Build sorted points");

		print_solution_stats(sol, octree.points->num_points);
		destroy_solution(sol);

		neighborhoods_tfg_bench(&points_sorted);

		destroy_points_sorted(&points_sorted);
	}

	destroy_octree(&octree);
}