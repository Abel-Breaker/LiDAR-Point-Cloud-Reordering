#include "neighborhood_algorithms/knn_bruteforce.h"
#include "neighborhood_algorithms/knn_kd_tree.h"
#include "structures/kd_tree.h"
#include "types/lidar_points.h"
#include "utils/error_handler.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
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

	destroy_kd_tree(&tree);
	destroy_points(&points);

	return 0;
}