#include "neighborhood_algorithms/knn_kd_tree.h"
#include "neighborhood_algorithms/knn_bruteforce.h"
#include "structures/kd_tree_optimized.h"
#include "types/lidar_points.h"
#include "utils/error_handler.h"
#include "utils/parse_args.h"
#include "utils/parse_lidar_points.h"
#include <stdio.h>
#include <math.h>

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


	size_t neighbours[K];
	start_kdtree_knearest(&tree, 0, neighbours);
	for (size_t j = 0; j < K; j++)
		printf("Neighbor %zu: %zu\n", j, neighbours[j]);

	destroy_kd_tree(&tree);


	find_point_neighbors(&points, 0, neighbours);

	for(size_t i=0; i<K; ++i){
		printf("Neighbor %zu: %zu\n", i, neighbours[i]);
	}

	destroy_points(&points);

	return 0;
}