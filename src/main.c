#define _POSIX_C_SOURCE 199309L
#include "neighborhood_algorithms/knn_bruteforce.h"
#include "neighborhood_algorithms/knn_kd_tree.h"
#include "neighborhood_algorithms/search_octree.h"
#include "points_structures/kd_tree.h"
#include "points_structures/octree.h"
#include "types/lidar_points.h"
#include "utils/error_handler.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
#include "points_reorder_algorithms/cuthill-mckee.h"
#include <stdlib.h> 
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

//#include <cstdint>

typedef void (*NeighborFunc)(const void *structure, size_t point_index, size_t *neighbours_index,
			     double *neighbours_distances);

void save_neighborhood_matrix_on_file(const Points *points, NeighborFunc neighbor_func, const void *structure, const char *filename)
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

void check_neighborhoods_calculation(const Points *points, NeighborFunc neighbor_func, const void *structure){

	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	for (size_t i = 0; i < 500; ++i) {
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		neighbor_func(structure, i, neighbours, neighbours_distances);
		find_point_neighbors(points, i, neighbours_2, neighbours_distances_2);
		for (size_t j = 0; j < K; j++) {
			//printf("%ld - %ld\n", neighbours[j], neighbours_2[j]);
			//printf("%f - %f\n", neighbours_distances[j], neighbours_distances_2[j]);
			//assert(neighbours[j] == neighbours_2[j]);
			// Check distances because there are indices that are at the same distance and can colide
			double diff = fabs(neighbours_distances[j] - neighbours_distances_2[j]);
            const double epsilon = 1e-2f;  // tolerancia para errores numéricos
            assert(diff < epsilon);
		}
	}
}

void create_kd_tree_benchmark(Points *points_reordered)
{
	struct timespec start, end;
	double total=0;

	// Create and check new kd_tree
	KDTree tree_2 = {};

	// WARM UP
	for (int i = 0; i < 3; ++i) {
		create_kd_tree(&tree_2, points_reordered);
		destroy_kd_tree(&tree_2);
	}

	for (int i = 0; i < 10; ++i) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		create_kd_tree(&tree_2, points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

		destroy_kd_tree(&tree_2);
	}

	printf("KD-TREE CREATION: %.6f s\n", total / 10);
}

void neighborhoods_benchmark_secuential(NeighborFunc neighbor_func, const void *structure)
{
	struct timespec start, end;
	double total=0;

	volatile double sink_dist=0;
	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (size_t i = 0; i < 100000; ++i) {
		neighbor_func(structure, i, neighbours, neighbours_distances);

		// Force use to avoid code elimination
		sink_dist += (double)neighbours[0];
		sink_dist += neighbours_distances[0];
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("Neighborhood secuential calculation: %.6f s\n", total);
	//printf("Dummy: %f\n", sink_dist);
	(void)sink_dist;
}

void neighborhoods_benchmark_random(NeighborFunc neighbor_func, const void *structure, size_t num_points)
{
	struct timespec start, end;
	double total=0;

	volatile double sink_dist=0;
	size_t neighbours[K];
	double neighbours_distances[K];

	srand((unsigned int)time(NULL));

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (size_t i = 0; i < 100000; ++i) {
		
		neighbor_func(structure, ((size_t)rand() % num_points), neighbours, neighbours_distances);

		// Force use to avoid code elimination
		sink_dist += (double)neighbours[0];
		sink_dist += neighbours_distances[0];
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("Neighborhood random calculation: %.6f s\n", total);
	//printf("Dummy: %f\n", sink_dist);
	(void)sink_dist;
}

int main(int argc, char **argv)
{
	struct timespec start, end;
	double total=0;


	// Arguments parse
	Args args = {};
	parse_args(argc, argv, &args);
	printf("filename: %s\n", args.cloud_points_file_name);

	// Read and save points
	Points points = {};
	if (read_las_points(args.cloud_points_file_name, &points) == false) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, nullptr);
	}
	//points.num_points = 100000;
	printf("%zu\n", points.num_points);

	// Benchmark KD-Tree
	printf("\nDEFAULT\n");
	create_kd_tree_benchmark(&points);

	// Create kdtree and check
	KDTree tree = {};
	create_kd_tree(&tree, &points);
	//check_kd_tree(&tree);
	//check_neighborhoods_calculation(&points, (const void *)start_kdtree_knearest, &tree);

	// Benchmark neighborhoods
	neighborhoods_benchmark_secuential((const void *)start_kdtree_knearest, &tree);
	neighborhoods_benchmark_random((const void *)start_kdtree_knearest, &tree, points.num_points);

	//save_neighborhood_matrix_on_file(&points, (const void *)start_kdtree_knearest, &tree, "../R/before.txt");

	// BFS SORT BY DISTANCE
	{
		printf("\nBFS SORT BY DISTANCE\n");
		// Reorder points
		Points points_reordered = {};

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		reorder_bfs_sort_by_distance(&tree, &points, &points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

		printf("SORT: %.6f s\n", total);
		total = 0;

		printf("Reordenado\n");

		// Benchmark KD-Tree
		create_kd_tree_benchmark(&points_reordered);

		// Create and check new kd_tree
		KDTree tree_2 = {};
		create_kd_tree(&tree_2, &points_reordered);
		//check_kd_tree(&tree_2);
		check_neighborhoods_calculation(&points_reordered, (const void *)start_kdtree_knearest, &tree_2);

		// Benchmark neighborhoods
		neighborhoods_benchmark_secuential((const void *)start_kdtree_knearest, &tree_2);
		neighborhoods_benchmark_random((const void *)start_kdtree_knearest, &tree_2, points_reordered.num_points);

		//save_neighborhood_matrix_on_file(&points_reordered, (const void *)start_kdtree_knearest, &tree_2, "../R/after_DISTANCE.txt");

		destroy_kd_tree(&tree_2);
		destroy_points(&points_reordered);
	}

	// BFS SORT BY INDEX
	{
		printf("\nBFS SORT BY INDEX\n");
		// Reorder points
		Points points_reordered = {};

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		reorder_bfs_sort_by_distance(&tree, &points, &points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

		printf("SORT: %.6f s\n", total);
		total = 0;

		printf("Reordenado\n");

		// Benchmark KD-Tree
		create_kd_tree_benchmark(&points_reordered);

		// Create and check new kd_tree
		KDTree tree_2 = {};
		create_kd_tree(&tree_2, &points_reordered);
		//check_kd_tree(&tree_2);
		check_neighborhoods_calculation(&points_reordered, (const void *)start_kdtree_knearest, &tree_2);

		// Benchmark neighborhoods
		neighborhoods_benchmark_secuential((const void *)start_kdtree_knearest, &tree_2);
		neighborhoods_benchmark_random((const void *)start_kdtree_knearest, &tree_2, points_reordered.num_points);

		//save_neighborhood_matrix_on_file(&points_reordered, (const void *)start_kdtree_knearest, &tree_2, "../R/after_INDEX.txt");

		destroy_kd_tree(&tree_2);
		destroy_points(&points_reordered);
	}

	// BFS SORT BY DISTANCE REVERSE
	{
		printf("\nBFS SORT BY DISTANCE REVERSE\n");
		// Reorder points
		Points points_reordered = {};

		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		reorder_bfs_sort_by_distance(&tree, &points, &points_reordered);
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		total = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;

		printf("SORT: %.6f s\n", total);
		//total = 0;

		printf("Reordenado\n");

		// Benchmark KD-Tree
		create_kd_tree_benchmark(&points_reordered);

		// Create and check new kd_tree
		KDTree tree_2 = {};
		create_kd_tree(&tree_2, &points_reordered);
		//check_kd_tree(&tree_2);
		check_neighborhoods_calculation(&points_reordered, (const void *)start_kdtree_knearest, &tree_2);

		// Benchmark neighborhoods
		neighborhoods_benchmark_secuential((const void *)start_kdtree_knearest, &tree_2);
		neighborhoods_benchmark_random((const void *)start_kdtree_knearest, &tree_2, points_reordered.num_points);

		//save_neighborhood_matrix_on_file(&points_reordered, (const void *)start_kdtree_knearest, &tree_2, "../R/after_DISTANCE_REV.txt");

		destroy_kd_tree(&tree_2);
		destroy_points(&points_reordered);
	}

	destroy_kd_tree(&tree);















	// Octree creation and check
	Octree octree = {};
	create_octree(&octree, &points);
	//octree_print_stats(&octree);
	//check_neighborhoods_calculation(&points, (const void *)start_octree_knearest, &octree);

	// Validación búsqueda por radio fijo: comparar con fuerza bruta
	// Usamos como radio la distancia al K-ésimo vecino del punto 0
	{
		size_t nbr0[K];
		double dist0[K];
		start_octree_knearest(&octree, 0, nbr0, dist0);
		double test_radius = dist0[K - 1];

		for (size_t i = 0; i < 10; ++i) {
			double px = points.x[i], py = points.y[i], pz = points.z[i];

			// Fuerza bruta
			size_t bf_count = 0;
			for (size_t j = 0; j < points.num_points; ++j) {
				double d = euclidian_distance_3d(points.x[j], points.y[j], points.z[j],
				                                 px, py, pz);
				if (d <= test_radius) bf_count++;
			}

			// Octree
			RadiusResult res = {};
			octree_radius_search(&octree, i, test_radius, &res);
			assert(res.count == bf_count);
			radius_result_destroy(&res);
		}
	}
	destroy_octree(&octree);

	destroy_points(&points);

	printf("Ok!\n");

	return 0;
}