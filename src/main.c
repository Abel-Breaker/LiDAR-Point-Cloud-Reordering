#define _POSIX_C_SOURCE 199309L
#include "benchmarks/bench.h"
#include "neighborhood_algorithms/knn/bruteforce.h"
#include "neighborhood_algorithms/knn/kd_tree.h"
#include "neighborhood_algorithms/radius_search/octree.h"
#include "neighborhood_algorithms/radius_search/octree_POC.h"
#include "points_reorder_algorithms/BFS.h"
#include "points_reorder_algorithms/cuthill-mckee.h"
#include "points_reorder_algorithms/random.h"
#include "points_structures/kd_tree.h"
#include "points_structures/octree.h"
#include "tests/test.h"
#include "types/lidar_points.h"
#include "types/neighborhood_matrix.h"
#include "types/neighborhood_matrix_mix/neighborhood_matrix_mix.h"
#include "utils/error_handler.h"
#include "utils/lidar_points_writer.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void save_matrix(const matrix_mix *matrix, const char *filename)
{
	FILE *fd;
	fd = fopen(filename, "w");

	size_t i = 0;

	for (i = 0; i < matrix->points->num_points; ++i) {
		size_t neighbours[K];
		get_neighbours_matrix_mix(matrix, i, neighbours);
		for (size_t k = 0; k < K; ++k) {
			fprintf(fd, "%zu ", neighbours[k]);
		}

		fprintf(fd, "\n");
	}

	printf("i : %zu\n", i);

	fclose(fd);
}
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

	// Tree for testing
	KDTree tree = {};
	create_kd_tree(&tree, &points);
	
	/*matrix_mix matrix = {};
	create_neighbourhood_matrix_mix(&matrix, &tree);
	printf("Matrix created\n");
	print_matrix_mix_stats(&matrix);
	//save_matrix(&matrix, "no_reorder.txt");
	printf("Matrix saved\n");
	destroy_neighbourhood_matrix_mix(&matrix);
	
	Points points_reordered = {0};

	reorder_bfs_sort_by_distance(&tree, tree.pts, &points_reordered);
	destroy_kd_tree(&tree);
	create_kd_tree(&tree, &points_reordered);
	matrix_mix matrix_2 = {};
	create_neighbourhood_matrix_mix(&matrix_2, &tree);
	printf("Matrix reordered created\n");
	print_matrix_mix_stats(&matrix_2);
	//save_matrix(&matrix_2, "reorder.txt");
	printf("Matrix reordered saved\n");
	destroy_neighbourhood_matrix_mix(&matrix_2);
	destroy_points(&points_reordered);*/


	/*
		{
			Octree octree = {};
			create_octree(&octree, &points);

			FILE *out = fopen("../R/same_leaf/radius_neighbors_same_leaf.txt", "w");
			if (!out) {
				fprintf(stderr, "Error: no se pudo abrir radius_neighbors_same_leaf.txt para
	   escritura\n"); destroy_octree(&octree); destroy_kd_tree(&tree); destroy_points(&points); return 1;
			}

			// Validación búsqueda por radio fijo: comparar con fuerza bruta
			// Usamos como radio la distancia al K-ésimo vecino del punto 0
			size_t nbr0[K];
			double dist0[K];
			start_octree_knearest(&octree, 0, nbr0, dist0);

			// Test neighborhood
			for (size_t i = 0; i < octree.pts->num_points; ++i) {
				RadiusResultPOC res = {};
				octree_radius_search_POC(&octree, i, get_args()->radius_search, &res);

				for (size_t j = 0; j < res.count; ++j) {
					fprintf(out, "%zu", res.indices[j]);
					if (j + 1 < res.count) fprintf(out, " ");
				}
				fprintf(out, ";");

				for (size_t j = 0; j < res.count; ++j) {
					if (!res.is_in_same_leaf[j]) continue;
					fprintf(out, " %zu", res.indices[j]);
				}
				fprintf(out, "\n");

				// Force use to avoid code elimination
				radius_result_destroy_POC(&res);
			}

			fclose(out);

			destroy_octree(&octree);
		}*/

	// DEFAULT

	{
		test_idea("DEFAULT", nullptr, &tree, &points);
		// test_idea("RANDOM REORDER", (SortFunc)reorder_random, &tree, &points);
		test_idea("BFS SORT BY DISTANCE", (SortFunc)reorder_bfs_sort_by_distance, &tree, &points);
		// test_idea("BFS SORT BY DISTANCE REVERSE", (SortFunc)reorder_bfs_sort_by_distance_reverse, &tree,
		// &points);
	}

	destroy_kd_tree(&tree);
	destroy_points(&points);

	return 0;
}