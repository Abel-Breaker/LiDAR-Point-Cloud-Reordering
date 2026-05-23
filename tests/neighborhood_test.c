#include "neighborhood_test.h"
#include "../src/points/points.h"
#include "../src/points_sorted/points_sorted.h"
#include "../src/points_sorted/opt/points_sorted_opt.h"
#include "../utils/auxiliar_structures/radius_result.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ITER 100

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

void check_neighborhoods_octree_radius(const Octree *octree)
{
	// Validación búsqueda por radio fijo: comparar con fuerza bruta
	// Usamos como radio la distancia al K-ésimo vecino del punto 0

	// #pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {
		size_t index = (size_t)rand() % octree->points->num_points;

		RadiusResult resbf = {};
		find_radius_neighbors(octree->points, index, &resbf);

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

void check_neighborhoods_tfg(const Points_TFG *points)
{

	// #pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {
		size_t index = (size_t)rand() % points->points->num_points;

		RadiusResult res = {};
		tfg_radius_search(points, index, &res);

		RadiusResult resbf = {};
		find_radius_neighbors(points->points, index, &resbf);

		// Ordenar ambos resultados
		sort_neighbors(res.indices, res.distances, res.count);
		sort_neighbors(resbf.indices, resbf.distances, resbf.count);

		if (resbf.count != res.count) {
			printf("Not the same number of neighbours for iteration %zu (point %zu): %zu - %zu\n", i, index, resbf.count, res.count);
			exit(-1);
		}

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

void check_neighborhoods_tfg_opt(const Points_TFG_opt *points)
{

	// #pragma omp parallel for
	for (size_t i = 0; i < ITER; ++i) {
		size_t index = (size_t)rand() % points->points->num_points;

		RadiusResult res = {};
		tfg_radius_search_opt(points, index, &res);

		RadiusResult resbf = {};
		find_radius_neighbors(points->points, index, &resbf);

		// Ordenar ambos resultados
		sort_neighbors(res.indices, res.distances, res.count);
		sort_neighbors(resbf.indices, resbf.distances, resbf.count);

		if (resbf.count != res.count) {
			printf("Not the same number of neighbours for iteration %zu (point %zu): %zu - %zu\n", i, index, resbf.count, res.count);
			exit(-1);
		}

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