#define _POSIX_C_SOURCE 199309L
#include "neighborhood_bench.h"
#include "../neighborhood_algorithms/knn_kd_tree.h"
#include "../neighborhood_algorithms/search_octree.h"
#include "../points_structures/kd_tree.h"
#include "../points_structures/octree.h"
#include "../utils/error_handler.h"
#include "../utils/parse_args.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define ITER 1000

void neighborhoods_kd_tree_knn_bench(const KDTree *structure)
{
	struct timespec start, end;
	double total = 0;

	volatile double sink_dist = 0;
	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (size_t i = 0; i < ITER; ++i) {
		start_kdtree_knearest(structure, i, neighbours, neighbours_distances);

		// Force use to avoid code elimination
		sink_dist += (double)neighbours[0];
		sink_dist += neighbours_distances[0];
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("Neighborhood KD-Tree: %.6f s\n", total);

	(void)sink_dist;
}

void neighborhoods_octree_knn_bench(const Octree *structure)
{
	struct timespec start, end;
	double total = 0;

	volatile double sink_dist = 0;
	size_t neighbours[K];
	double neighbours_distances[K];

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (size_t i = 0; i < ITER; ++i) {
		start_octree_knearest(structure, i, neighbours, neighbours_distances);

		// Force use to avoid code elimination
		sink_dist += (double)neighbours[0];
		sink_dist += neighbours_distances[0];
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("Neighborhood Octree Knn: %.6f s\n", total);

	(void)sink_dist;
}

void neighborhoods_octree_radius_bench(const Octree *structure)
{
	struct timespec start, end;
	double total = 0;

	volatile double sink_dist = 0;

	// Validación búsqueda por radio fijo: comparar con fuerza bruta
	// Usamos como radio la distancia al K-ésimo vecino del punto 0
	size_t nbr0[K];
	double dist0[K];
	start_octree_knearest(structure, 0, nbr0, dist0);
	double test_radius = dist0[K - 1];

	// Test neighborhood
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	for (size_t i = 0; i < ITER; ++i) {
		RadiusResult res = {};
		octree_radius_search(structure, i, test_radius, &res);
		// Force use to avoid code elimination
		sink_dist += (double)res.distances[0];
		radius_result_destroy(&res);
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &end);
	total += (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
	printf("Neighborhood Octree radius search: %.6f s\n", total);

	(void)sink_dist;
}