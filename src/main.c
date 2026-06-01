#define _POSIX_C_SOURCE 199309L
#include "../bench/bench.h"
#include "../tests/test.h"
#include "octree/octree.h"
#include "points/points.h"
#include "reorder/cuthill-mckee.h"
#include "../utils/error_handler.h"
#include "../utils/io/lidar/points_reader.h"
#include "../utils/parse_args.h"
#include "../utils/terminal_formating.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{
	// Arguments parse
	if(!parse_args(argc, argv)){
		handle_error(ERROR_PARSE_ARG, ERR_FATAL, "Cannot parse args");
	}

	// Read and save points
	Points points = {};
	if (!read_las_points(get_args()->cloud_points_file_name, &points)) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, "Cannot read points from file");
	}

	// Print some info
	printf("%s\n\nFilename:%s %s\n", BOLD, COLOR_RESET, get_args()->cloud_points_file_name);
	printf("%sNumber of points:%s %zu\n", BOLD, COLOR_RESET, (size_t) points.num_points);
	printf("%sRadius reorder:%s %f\n", BOLD, COLOR_RESET, (double)get_args()->radius_reorder);
	printf("%sRadius search:%s %f\n", BOLD, COLOR_RESET, (double)get_args()->radius_search);

	if (get_args()->do_benchmark)
		bench(&points);
	if (get_args()->do_test)
		test(&points);

	destroy_points(&points);

	return 0;
}