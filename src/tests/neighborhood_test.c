#include "../neighborhood_algorithms/knn_kd_tree.h"
#include "../neighborhood_algorithms/knn_kd_tree_prune.h"
#include "../neighborhood_algorithms/search_octree.h"
#include "../neighborhood_algorithms/knn_bruteforce.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define ITER 500

void check_neighborhoods_kd_tree(const KDTree *tree)
{

	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	for (size_t i = 0; i < ITER; ++i) {
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		start_kdtree_knearest(tree, i, neighbours, neighbours_distances);
		find_point_neighbors(tree->pts, i, neighbours_2, neighbours_distances_2);
		for (size_t j = 0; j < K; j++) {
			//printf("%ld - %ld\n", neighbours[j], neighbours_2[j]);
			//printf("%f - %f\n", neighbours_distances[j], neighbours_distances_2[j]);
			// assert(neighbours[j] == neighbours_2[j]);
			//  Check distances because there are indices that are at the same distance and can colide
			double diff = fabs(neighbours_distances[j] - neighbours_distances_2[j]);
			const double epsilon = 1e-2f; // tolerancia para errores numéricos
			assert(diff < epsilon);
		}
	}
}

void check_neighborhoods_kd_tree_prune(const KDTreePrune *tree)
{

	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	for (size_t i = 0; i < ITER; ++i) {
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		start_kdtree_prune_knearest(tree, i, neighbours, neighbours_distances);
		find_point_neighbors(tree->pts, i, neighbours_2, neighbours_distances_2);
		for (size_t j = 0; j < K; j++) {
			//printf("%ld - %ld\n", neighbours[j], neighbours_2[j]);
			//printf("%f - %f\n", neighbours_distances[j], neighbours_distances_2[j]);
			// assert(neighbours[j] == neighbours_2[j]);
			//  Check distances because there are indices that are at the same distance and can colide
			double diff = fabs(neighbours_distances[j] - neighbours_distances_2[j]);
			const double epsilon = 1e-2f; // tolerancia para errores numéricos
			assert(diff < epsilon);
		}
	}
}

void check_neighborhoods_octree_knn(const Octree *octree)
{

	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	for (size_t i = 0; i < ITER; ++i) {
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		start_octree_knearest(octree, i, neighbours, neighbours_distances);
		find_point_neighbors(octree->pts, i, neighbours_2, neighbours_distances_2);
		for (size_t j = 0; j < K; j++) {
			// printf("%ld - %ld\n", neighbours[j], neighbours_2[j]);
			// printf("%f - %f\n", neighbours_distances[j], neighbours_distances_2[j]);
			// assert(neighbours[j] == neighbours_2[j]);
			//  Check distances because there are indices that are at the same distance and can
			//  colide
			double diff = fabs(neighbours_distances[j] - neighbours_distances_2[j]);
			const double epsilon = 1e-2f; // tolerancia para errores numéricos
			assert(diff < epsilon);
		}
	}
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