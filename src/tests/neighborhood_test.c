#include "neighborhood_test.h"
#include "../neighborhood_algorithms/knn/bruteforce.h"
#include "../neighborhood_algorithms/radius_search/bruteforce.h"
#include "../neighborhood_algorithms/radius_search/octree.h"
#include "../utils/auxiliar_structures/radius_result.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITER 1000

typedef void (*NeighborFunc)(const void *structure, size_t point_index, size_t *neighbours_index,
			     double *neighbours_distances);

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

	// #pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {

		size_t index = (size_t)rand() % pts->num_points;
		size_t neighbours_2[K];
		double neighbours_distances_2[K];

		function(structure, index, neighbours, neighbours_distances);
		find_knn_neighbors(pts, index, neighbours_2, neighbours_distances_2);

		// Ordenar ambos resultados
		sort_neighbors(neighbours, neighbours_distances, K);
		sort_neighbors(neighbours_2, neighbours_distances_2, K);

		// Comparar
		for (size_t j = 0; j < K; j++) {
			if (neighbours[j] != neighbours_2[j]) {

				const double epsilon = 1e-5;
				double diff = fabs(neighbours_distances[j] - neighbours_distances_2[j]);

				if (diff > epsilon) {
					printf("%zu (%f) - %zu (%f) | diff = %f\n", neighbours[j],
					       neighbours_distances[j], neighbours_2[j], neighbours_distances_2[j],
					       diff);
					exit(-1);
				}
			}
		}
	}
}

void check_neighborhoods_octree_knn(const Octree *octree)
{
	check_neighborhoods_knn((NeighborFunc)start_octree_knearest, octree, octree->pts);
}

void check_neighborhoods_octree_radius(const Octree *octree)
{
	// Validación búsqueda por radio fijo: comparar con fuerza bruta
	// Usamos como radio la distancia al K-ésimo vecino del punto 0

	// #pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {
		size_t index = (size_t)rand() % octree->pts->num_points;

		RadiusResult resbf = {};
		find_radius_neighbors(octree->pts, index, &resbf);

		// Octree
		RadiusResultOctree res = {};
		octree_radius_search(octree, index, get_args()->radius_search, &res);

		if (resbf.count != res.count) {
			printf("Not the same number of neighbours for iteration %zu (point %zu): %zu - %zu\n", i, index, resbf.count, res.count);
			exit(-1);
		}

		if (resbf.count == 0) {
			continue;
		}

		sort_neighbors(resbf.indices, resbf.distances, resbf.count);
		sort_neighbors(res.indices, res.distances, res.count);

		// Comparar
		for (size_t j = 0; j < res.count; j++) {
			if (resbf.indices[j] != res.indices[j]) {

				const double epsilon = 1e-5;
				double diff = fabs(resbf.distances[j] - res.distances[j]);

				if (diff > epsilon) {
					printf("%zu (%f) - %zu (%f) | diff = %f\n", resbf.indices[j],
					       resbf.distances[j], res.indices[j], res.distances[j], diff);
					exit(-1);
				}
			}
		}

		destroy_radius_result(&resbf);
		radius_result_destroy(&res);
	}
}

// TODO:
void check_neighborhoods_matrix_mix(const struct matrix_t *matrix)
{

	// #pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {
		size_t index = (size_t)rand() % matrix->points->num_points;

		RadiusResult res = {};
		get_neighbours_matrix(matrix, index, &res);

		RadiusResult resbf = {};
		find_radius_neighbors(matrix->points, index, &resbf);

		// Ordenar ambos resultados
		sort_neighbors(res.indices, res.distances, res.count);
		sort_neighbors(resbf.indices, resbf.distances, resbf.count);

		// Comparar
		for (size_t j = 0; j < res.count; j++) {
			if (res.indices[j] != resbf.indices[j]) {
				const double epsilon = 1e-5;
				double diff = fabs(resbf.distances[j] - res.distances[j]);

				if (diff > epsilon) {
					printf("%zu (%f) - %zu (%f)\n", res.indices[j], res.distances[j],
					       resbf.indices[j], resbf.distances[j]);
					destroy_radius_result(&resbf);
					destroy_radius_result(&res);
					exit(-1);
				}
			}
		}
		destroy_radius_result(&resbf);
		destroy_radius_result(&res);
	}
}