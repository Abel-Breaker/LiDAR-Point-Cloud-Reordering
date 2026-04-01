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
#include <assert.h>
#include <math.h>
#include <stdio.h>

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
            const double epsilon = 1e-6f;  // tolerancia para errores numéricos
            assert(diff < epsilon);
		}
	}
}

int main(int argc, char **argv)
{
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

	// Create kdtree and check
	KDTree tree = {};
	create_kd_tree(&tree, &points);
	check_kd_tree(&tree);
	check_neighborhoods_calculation(&points, (const void *)start_kdtree_knearest, &tree);
	//save_neighborhood_matrix_on_file(&points, (const void *)start_kdtree_knearest, &tree, "../R/before.txt");

	// BFS SORT BY DISTANCE
	{
		printf("BFS SORT BY DISTANCE\n");
		// Reorder points
		Points points_reordered = {};
		reorder_bfs_sort_by_distance(&tree, &points, &points_reordered);
		printf("Reordenado\n");

		// Create and check new kd_tree
		KDTree tree_2 = {};
		create_kd_tree(&tree_2, &points_reordered);
		check_kd_tree(&tree_2);
		check_neighborhoods_calculation(&points_reordered, (const void *)start_kdtree_knearest, &tree_2);

		//save_neighborhood_matrix_on_file(&points_reordered, (const void *)start_kdtree_knearest, &tree_2, "../R/after_DISTANCE.txt");

		destroy_kd_tree(&tree_2);
		destroy_points(&points_reordered);
	}

	// BFS SORT BY INDEX
	{
		printf("BFS SORT BY INDEX\n");
		// Reorder points
		Points points_reordered = {};
		reorder_bfs_sort_by_index(&tree, &points, &points_reordered);
		printf("Reordenado\n");

		// Create and check new kd_tree
		KDTree tree_2 = {};
		create_kd_tree(&tree_2, &points_reordered);
		check_kd_tree(&tree_2);
		check_neighborhoods_calculation(&points_reordered, (const void *)start_kdtree_knearest, &tree_2);

		//save_neighborhood_matrix_on_file(&points_reordered, (const void *)start_kdtree_knearest, &tree_2, "../R/after_INDEX.txt");

		destroy_kd_tree(&tree_2);
		destroy_points(&points_reordered);
	}

	// BFS SORT BY DISTANCE REVERSE
	{
		printf("BFS SORT BY DISTANCE REVERSE\n");
		// Reorder points
		Points points_reordered = {};
		reorder_bfs_sort_by_distance_reverse(&tree, &points, &points_reordered);
		printf("Reordenado\n");

		// Create and check new kd_tree
		KDTree tree_2 = {};
		create_kd_tree(&tree_2, &points_reordered);
		check_kd_tree(&tree_2);
		check_neighborhoods_calculation(&points_reordered, (const void *)start_kdtree_knearest, &tree_2);

		//save_neighborhood_matrix_on_file(&points_reordered, (const void *)start_kdtree_knearest, &tree_2, "../R/after_DISTANCE_REV.txt");

		destroy_kd_tree(&tree_2);
		destroy_points(&points_reordered);
	}

	destroy_kd_tree(&tree);















	// Octree creation and check
	Octree octree = {};
	create_octree(&octree, &points);
	//octree_print_stats(&octree);
	check_neighborhoods_calculation(&points, (const void *)start_octree_knearest, &octree);

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