#define _POSIX_C_SOURCE 199309L
#include "benchmarks/bench.h"
#include "neighborhood_algorithms/knn_bruteforce.h"
#include "neighborhood_algorithms/knn_kd_tree.h"
#include "neighborhood_algorithms/search_octree.h"
#include "points_reorder_algorithms/cuthill-mckee.h"
#include "points_structures/kd_tree.h"
#include "points_structures/octree.h"
#include "tests/test.h"
#include "types/lidar_points.h"
#include "utils/error_handler.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*typedef void (*NeighborFunc)(const void *structure, size_t point_index, size_t *neighbours_index,
			     double *neighbours_distances);

void save_neighborhood_matrix_on_file(const Points *points, NeighborFunc neighbor_func, const void *structure,
				      const char *filename)
{
	FILE *fd;
	fd = fopen(filename, "w");

	for (size_t i = 0; i < points->num_points; ++i) {
		size_t neighbours[K];
		double neighbours_distances[K];

		neighbor_func(structure, i, neighbours, neighbours_distances);

		for (size_t j = 0; j < points->num_points; ++j) {
			int is_neighbour = 0;
			for (size_t k = 0; k < K; ++k) {
				if (neighbours[k] == j) {
					is_neighbour = 1;
					break;
				}
			}
			fprintf(fd, "%d ", is_neighbour);
		}
		fprintf(fd, "\n");
	}

	fclose(fd);
}

save_neighborhood_matrix_on_file(&points, (const void *)start_kdtree_knearest, &tree,
		// "../R/before.txt");
*/

int main(int argc, char **argv)
{
	struct timespec start, end;
	double total = 0;

	// Arguments parse
	Args args = {};
	parse_args(argc, argv, &args);
	printf("filename: %s\n", args.cloud_points_file_name);

	// Read and save points
	Points points = {};
	if (read_las_points(args.cloud_points_file_name, &points) == false) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, nullptr);
	}
	//points.num_points = 10000;
	printf("%zu\n", points.num_points);

	// Tree for testing
	KDTree tree = {};
	create_kd_tree(&tree, &points);

	// DEFAULT
	{
		printf("\n### DEFAULT ###\n");
		if (args.do_benchmark){
			bench(&points);
		}
		if (args.do_test){
			test(&points);
		}
	}

	// BFS SORT BY DISTANCE
	{
		printf("\n### BFS SORT BY DISTANCE ###\n");

		// Reorder points
		Points points_reordered = {};
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		reorder_bfs_sort_by_distance(&tree, &points, &points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
		printf("SORT: %.6f s\n", total);

		if (args.do_benchmark){
			bench(&points_reordered);
		}
		if (args.do_test){
			test(&points_reordered);
		}

		destroy_points(&points_reordered);
	}

	// BFS SORT BY INDEX
	{
		printf("\n### BFS SORT BY INDEX ###\n");

		// Reorder points
		Points points_reordered = {};
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		reorder_bfs_sort_by_index(&tree, &points, &points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
		printf("SORT: %.6f s\n", total);

		if (args.do_benchmark){
			bench(&points_reordered);
		}
		if (args.do_test){
			test(&points_reordered);
		}

		destroy_points(&points_reordered);
	}

	// BFS SORT BY DISTANCE REVERSE
	{
		printf("\n### BFS SORT BY DISTANCE REVERSE ###\n");

		// Reorder points
		Points points_reordered = {};
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		reorder_bfs_sort_by_distance_reverse(&tree, &points, &points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
		printf("SORT: %.6f s\n", total);

		if (args.do_benchmark){
			bench(&points_reordered);
		}
		if (args.do_test){
			test(&points_reordered);
		}

		destroy_points(&points_reordered);
	}

	destroy_kd_tree(&tree);
	destroy_points(&points);

	return 0;
}