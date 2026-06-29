#define _POSIX_C_SOURCE 199309L
#include "../bench/bench.h"
#include "../tests/test.h"
#include "../utils/error_handler.h"
#include "../utils/io/lidar/points_reader.h"
#include "../utils/io/lidar/points_writer.h"
#include "../utils/parse_args.h"
#include "../utils/terminal_formating.h"
#include "octree/octree.h"
#include "points/points.h"
#include "points_sorted/points_sorted.h"
#include "reorder/cuthill-mckee.h"
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main(int argc, char **argv)
{
	// Arguments parse
	if (!parse_args(argc, argv)) {
		handle_error(ERROR_PARSE_ARG, ERR_FATAL, "Cannot parse args");
	}

	// Read and save points
	Points points = {};
	if (!read_las_points(get_args()->cloud_points_file_name, &points)) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, "Cannot read points from file");
	}

	// Print some info
	printf("%s\n\nFilename:%s %s\n", BOLD, COLOR_RESET, get_args()->cloud_points_file_name);
	printf("%sNumber of points:%s %zu\n", BOLD, COLOR_RESET, (size_t)points.num_points);
	printf("%sRadius reorder:%s %f\n", BOLD, COLOR_RESET, (double)get_args()->radius_reorder);
	printf("%sRadius search:%s %f\n", BOLD, COLOR_RESET, (double)get_args()->radius_search);

	
	if (get_args()->do_benchmark)
		bench(&points);
	if (get_args()->do_test)
		test(&points);
	if (get_args()->save_ordered_cloud) {
		Octree octree = {};
		create_octree(&octree, &points);

		Solution *sol = reorder_cuthill_mckee(&octree);
		Points_sorted points_sorted = {};
		build_sorted_points(&points_sorted, octree.points, sol);
		print_solution_stats(sol, octree.points->num_points);

		write_las_points(get_args()->save_ordered_cloud, &points_sorted);

		destroy_solution(sol);
		destroy_points_sorted(&points_sorted);
		destroy_octree(&octree);
	}

	destroy_points(&points);

	return 0;
}