#include "neighborhood_algorithms/knn_bruteforce.h"
#include "neighborhood_algorithms/knn_kd_tree.h"
#include "neighborhood_algorithms/search_octree.h"
#include "structures/kd_tree.h"
#include "structures/octree.h"
#include "types/lidar_points.h"
#include "utils/error_handler.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
#include "points_reorder_algorithms/structures/neighborhood_matrix.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	Args args = {};

	parse_args(argc, argv, &args);

	printf("filename: %s\n", args.cloud_points_file_name);

	Points points = {};

	if (read_las_points(args.cloud_points_file_name, &points) == false) {
		handle_error(ERROR_PARSE_POINTS, ERR_FATAL, nullptr);
	}

	printf("%zu\n", points.num_points);

	KDTree tree = {};
	create_kd_tree(&tree, &points);

	// Verifica que el mediano está bien colocado en el eje X
	size_t mid = points.num_points / 2;
	printf("Indice de la mediana: %zu - Indice root: %zu\n", tree.indices[mid], tree.root->point_index);

	// Comprueba que todos los de la izquierda son <= mediano
	double mid_val = points.x[tree.indices[mid]];
	int ok = 1;
	for (size_t i = 0; i < mid; i++) {
		if (points.x[tree.indices[i]] > mid_val) {
			ok = 0;
			break;
		}
	}
	printf("Particion correcta: %s\n", ok ? "SI" : "NO");

	int valid = verify_kd_node(&tree, 0, points.num_points, 0);
	printf("Arbol valido: %s\n", valid ? "SI" : "NO");

	check_number_of_nodes(&tree);
/*
	FILE *fd;
	fd = fopen("adjacency_matrix.txt", "w");

	for (size_t i = 0; i < points.num_points; ++i) {
		size_t neighbours[K];
		double neighbours_distances[K];

		start_kdtree_knearest(&tree, i, neighbours, neighbours_distances);

		for (size_t j = 0; j < points.num_points; ++j) {
			int is_neighbour = 0;
			for (size_t k = 0; k < K; ++k) {
				if (neighbours[k] == j) {
					is_neighbour = 1;
					break;
				}
			}
			if (j < points.num_points - 1)
				fprintf(fd, "%d ", is_neighbour);
			else
				fprintf(fd, "%d", is_neighbour);
		}
		fprintf(fd, "\n");
	}

	fclose(fd);*/

	// Test neighborhood
	for (size_t i = 0; i < 10; ++i) {
		size_t neighbours[K];
		double neighbours_distances[K];
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		start_kdtree_knearest(&tree, i, neighbours, neighbours_distances);
		find_point_neighbors(&points, i, neighbours_2, neighbours_distances_2);
		for (size_t j = 0; j < K; j++) {
			assert(neighbours[j] == neighbours_2[j]);
		}
	}


	// Check spare matrix
	Matrix matrix;
	create_neighborhood_matrix(&tree, &matrix);

	size_t neighbours[K];
	double neighbours_distances[K];
	start_kdtree_knearest(&tree, 8, neighbours, neighbours_distances);
	for (size_t j = 0; j < K; j++) {
		assert(neighbours[j] == matrix[K*8+j]);
	}

	destroy_neighborhood_matrix(matrix);
	destroy_kd_tree(&tree);

/*
	// Octree test
	Octree octree = {};
	create_octree(&octree, &points);
	octree_print_stats(&octree);

	// Validación: los K vecinos del octree deben coincidir con fuerza bruta
	for (size_t i = 0; i < 10; ++i) {
		size_t neighbours_oct[K];
		double neighbours_distances_oct[K];
		size_t neighbours_bf[K];
		double neighbours_distances_bf[K];

		start_octree_knearest(&octree, i, neighbours_oct, neighbours_distances_oct);
		find_point_neighbors(&points, i, neighbours_bf, neighbours_distances_bf);
		for (size_t j = 0; j < K; j++) {
			assert(neighbours_oct[j] == neighbours_bf[j]);
		}
	}
	printf("Octree KNN: OK\n");

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
	printf("Octree Radio: OK\n");

	destroy_octree(&octree);*/

	destroy_points(&points);



	return 0;
}