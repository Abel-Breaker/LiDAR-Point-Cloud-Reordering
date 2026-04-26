#define _POSIX_C_SOURCE 199309L
#include "benchmarks/bench.h"
#include "neighborhood_algorithms/knn/bruteforce.h"
#include "neighborhood_algorithms/radius_search/octree.h"
#include "points_reorder_algorithms/BFS.h"
#include "points_reorder_algorithms/cuthill-mckee.h"
#include "points_reorder_algorithms/random.h"
#include "points_structures/octree.h"
#include "tests/test.h"
#include "types/lidar_points.h"
#include "types/neighborhood_matrix_mix/neighborhood_matrix.h"
#include "utils/error_handler.h"
#include "utils/lidar_points_writer.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*
void save_matrix(const struct matrix_t *matrix, const char *filename)
{
	FILE *fd;
	fd = fopen(filename, "w");

	size_t i = 0;

	for (i = 0; i < matrix->points->num_points; ++i) {
		size_t neighbours[K];
		get_neighbours_matrix(matrix, i, neighbours);
		for (size_t k = 0; k < K; ++k) {
			fprintf(fd, "%zu ", neighbours[k]);
		}

		fprintf(fd, "\n");
	}

	printf("i : %zu\n", i);

	fclose(fd);
}*/

typedef void (*SortFunc)(const void *structure, const Points *points, Points *new_points);

void test_idea(const char *name, SortFunc sort_fun, const void *structure, Points *points)
{
	printf("\n\x1b[34m\033[1m%s\033[0m\x1b[0m\n", name);

	const Args *args = get_args();

	// Case without sort
	if (sort_fun == NULL) {
		if (args->do_benchmark)
			bench(points);
		if (args->do_test)
			test(points);
		return;
	}

	struct timespec start, end;

	Points points_reordered = {0};

	clock_gettime(CLOCK_MONOTONIC, &start);
	sort_fun(structure, points, &points_reordered);
	clock_gettime(CLOCK_MONOTONIC, &end);

	double total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
	printf("\tSort: %.6f s\n", total);

	if (args->do_benchmark)
		bench(&points_reordered);
	if (args->do_test)
		test(&points_reordered);

	destroy_points(&points_reordered);
}

int main(int argc, char **argv)
{

	// Arguments parse
	parse_args(argc, argv);
	printf("\033[1mFilename\033[0m: %s\n", get_args()->cloud_points_file_name);

	// Read and save points
	Points points = {};
	if (read_las_points(get_args()->cloud_points_file_name, &points) == false) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, nullptr);
	}
	printf("\033[1mNumber of points: \033[0m%zu\n", points.num_points);
	printf("\033[1mNumber of neighbours calculated: \033[0m%zu\n", (size_t)K);

	if (get_args()->do_benchmark)
		bench(&points);
	if (get_args()->do_test)
		test(&points);

	{
		// test_idea("DEFAULT", nullptr, &tree, &points);
	}

	destroy_points(&points);

	return 0;
}