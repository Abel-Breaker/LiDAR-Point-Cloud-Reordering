#define _POSIX_C_SOURCE 199309L
#include "../bench/bench.h"
#include "../tests/test.h"
#include "octree/octree.h"
#include "points/points.h"
#include "reorder/cuthill-mckee.h"
#include "../utils/error_handler.h"
#include "../utils/io/lidar/points_reader.h"
#include "../utils/parse_args.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv)
{
	// Arguments parse
	parse_args(argc, argv);

	// Read and save points
	Points points = {};
	if (read_las_points(get_args()->cloud_points_file_name, &points) == false) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, nullptr);
	}

	// Print some info
	printf("\n\n\033[1mFilename\033[0m: %s\n", get_args()->cloud_points_file_name);
	printf("\033[1mNumber of points: \033[0m%zu\n", points.num_points);
	printf("\033[1mNumber of KNN neighbours: \033[0m%zu\n", (size_t)K);
	printf("\033[1mRadius: \033[0m%f\n", get_args()->radius_search);

	if (get_args()->do_benchmark)
		bench(&points);
	if (get_args()->do_test)
		test(&points);

	destroy_points(&points);

	return 0;
}