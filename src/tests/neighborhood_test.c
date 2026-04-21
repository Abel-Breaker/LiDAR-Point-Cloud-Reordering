#include "neighborhood_test.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../neighborhood_algorithms/knn/kd_tree_prune.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../neighborhood_algorithms/knn/bruteforce.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <time.h> 

#define ITER 1000

typedef void (*NeighborFunc)(const void *structure, size_t point_index, size_t *neighbours_index, double *neighbours_distances);


static void sort_neighbors(size_t *idx, double *dist, size_t k)
{
	for (size_t i = 0; i < k - 1; i++) {
		for (size_t j = i + 1; j < k; j++) {
			if (idx[j] < idx[i]) {
				// swap dist
				double dtmp = dist[i];
				dist[i] = dist[j];
				dist[j] = dtmp;

				// swap idx
				size_t itmp = idx[i];
				idx[i] = idx[j];
				idx[j] = itmp;
			}
		}
	}
}

static void check_neighborhoods_knn(NeighborFunc function, const void *structure, const Points *pts)
{
	size_t neighbours[K];
	double neighbours_distances[K];

	srand((unsigned)time(NULL));

	//#pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {

		size_t index = (size_t)rand() % pts->num_points;
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		function(structure, index, neighbours, neighbours_distances);
		find_point_neighbors(pts, index, neighbours_2, neighbours_distances_2);

		// Ordenar ambos resultados
		sort_neighbors(neighbours, neighbours_distances, K);
		sort_neighbors(neighbours_2, neighbours_distances_2, K);

		// Comparar
		for (size_t j = 0; j < K; j++) {
			if (neighbours[j] != neighbours_2[j]){
				printf("%zu (%f) - %zu (%f)\n", neighbours[j], neighbours_distances[j], neighbours_2[j], neighbours_distances_2[j]);
				exit(-1);
			}
		}
	}
}


void check_neighborhoods_kd_tree(const KDTree *tree)
{
	check_neighborhoods_knn((NeighborFunc)start_kdtree_knearest, tree, tree->pts);
}

void check_neighborhoods_kd_tree_prune(const KDTreePrune *tree)
{
	check_neighborhoods_knn((NeighborFunc)start_kdtree_prune_knearest, tree, tree->pts);
}

void check_neighborhoods_octree_knn(const Octree *octree)
{
	check_neighborhoods_knn((NeighborFunc)start_octree_knearest, octree, octree->pts);
}

void check_neighborhoods_octree_radius(const Octree *octree)
{
	// Validación búsqueda por radio fijo: comparar con fuerza bruta
	// Usamos como radio la distancia al K-ésimo vecino del punto 0
	{
		size_t nbr0[K];
		double dist0[K];
		start_octree_knearest(octree, 0, nbr0, dist0);
		double test_radius = dist0[K - 1];

		//#pragma omp parallel for
		for (size_t i = 0; i < ITER; ++i) {
			double px = octree->pts->x[i], py = octree->pts->y[i], pz = octree->pts->z[i];

			// Fuerza bruta
			size_t bf_count = 0;
			for (size_t j = 0; j < octree->pts->num_points; ++j) {
				double d = euclidian_distance_3d(octree->pts->x[j], octree->pts->y[j], octree->pts->z[j], px, py, pz);
				if (d <= test_radius)
					bf_count++;
			}

			// Octree
			RadiusResult res = {};
			octree_radius_search(octree, i, test_radius, &res);
			assert(res.count == bf_count);
			radius_result_destroy(&res);
		}
	}
}

void check_neighborhoods_matrix_mix(const matrix_mix *matrix)
{
	size_t neighbours[K];
	double neighbours_distances[K];

	//#pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		get_neighbours_matrix_mix(matrix, i, neighbours);
		find_point_neighbors(matrix->points, i, neighbours_2, neighbours_distances_2);

		// Ordenar ambos resultados
		sort_neighbors(neighbours, neighbours_distances, K);
		sort_neighbors(neighbours_2, neighbours_distances_2, K);

		// Comparar
		for (size_t j = 0; j < K; j++) {
			if (neighbours[j] != neighbours_2[j]){
				printf("%zu (%f) - %zu (%f)\n", neighbours[j], neighbours_distances[j], neighbours_2[j], neighbours_distances_2[j]);
				exit(-1);
			}
		}
	}
}