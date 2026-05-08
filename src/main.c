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

//Use: save_matrix(&matrix, "../R/reorder/data/reorder_4.txt");
void save_matrix(const struct matrix_t *matrix, const char *filename)
{
	FILE *fd;
	fd = fopen(filename, "w");
	if (!fd) {
		printf("Error opening\n");
		return;
	}

	size_t i = 0;

	for (i = 0; i < matrix->points->num_points; ++i) {
		RadiusResult res = {};
		get_neighbours_matrix(matrix, i, &res);
		for (size_t k = 0; k < res.count; ++k) {
			fprintf(fd, "%zu ", res.indices[k]);
		}
		destroy_radius_result(&res);

		fprintf(fd, "\n");
	}

	fclose(fd);
}

typedef void (*SortFunc)(const void *structure, Points *new_points);

void test_idea(const char *name, SortFunc sort_fun, const void *structure, const Points *points)
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
	sort_fun(structure, &points_reordered);
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

	{
		// Create necesary structures
		struct matrix_t matrix = {};
		Octree octree = {};
		create_octree(&octree, &points);
		create_neighbourhood_matrix(&matrix, &octree);
		print_matrix_stats(&matrix);

		

		// Test
		//test_idea("DEFAULT", nullptr, &matrix, matrix.points);
		test_idea("CUTHILL-MCKEE", (SortFunc)reorder_cuthill_mckee, &octree, octree.points);

		
		destroy_neighbourhood_matrix(&matrix);
		destroy_octree(&octree);
		destroy_points(&points);
	}

	return 0;
}