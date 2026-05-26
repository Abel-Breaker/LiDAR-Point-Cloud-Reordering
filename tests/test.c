#include "test.h"
#include "../src/octree/octree.h"
#include "../src/points_sorted/opt/points_sorted_opt.h"
#include "../src/points_sorted/points_sorted.h"
#include "../src/reorder/cuthill-mckee.h"
#include "../utils/terminal_formating.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void sort_neighbors(index_t *idx, data_t *dist, index_t k)
{
	for (index_t i = 0; i < k - 1; i++) {
		for (index_t j = i + 1; j < k; j++) {
			if (idx[j] < idx[i]) {
				// swap dist
				data_t dtmp = dist[i];
				dist[i] = dist[j];
				dist[j] = dtmp;

				// swap idx
				index_t itmp = idx[i];
				idx[i] = idx[j];
				idx[j] = itmp;
			}
		}
	}
}

static void test_points_for_octree(const Points *points, index_t indices[NUM_OF_TESTS],
				   RadiusResult res_1[NUM_OF_TESTS])
{
	// Octree creation
	Octree octree = {};
	create_octree(&octree, points);

// #pragma omp parallel for
#pragma omp parallel
	{
		RadiusResultOctree res_2 = {};

#pragma omp for
		for (index_t i = 0; i < NUM_OF_TESTS; ++i) {

			octree_radius_search(&octree, indices[i], get_args()->radius_search, &res_2);
			sort_neighbors(res_2.indices, res_2.distances, res_2.count);

			if (res_1[i].count != res_2.count) {
				printf(RED "Not the same number of neighbours for iteration %zu (point %zu): %zu - "
					   "%zu\n" COLOR_RESET,
				       (size_t)i, (size_t)indices[i], (size_t)res_1[i].count, (size_t)res_2.count);
			}

			// Comparar
			for (index_t j = 0; j < res_2.count; j++) {
				if (res_1[i].indices[j] != res_2.indices[j]) {

					const data_t epsilon = (data_t)1e-5;
					data_t diff = (data_t)fabs(res_1[i].distances[j] - res_2.distances[j]);

					if (diff > epsilon) {
						printf(RED "%zu (%f) - %zu (%f) | diff = %f\n" COLOR_RESET,
						       (size_t)res_1[i].indices[j], (double)res_1[i].distances[j],
						       (size_t)res_2.indices[j], (double)res_2.distances[j], (double)diff);
						exit(-1);
					}
				}
			}
		}
		radius_result_destroy(&res_2);
	}
	destroy_octree(&octree);
}

static void test_points_for_tfg(const Points_TFG *points, index_t indices[NUM_OF_TESTS],
				   RadiusResult res_1[NUM_OF_TESTS])
{
#pragma omp parallel for
	for (index_t i = 0; i < NUM_OF_TESTS; ++i) {
		RadiusResult res_2 = {};

		tfg_radius_search(points, indices[i], &res_2);
		sort_neighbors(res_2.indices, res_2.distances, res_2.count);

		// Check for same num of neighbours
		if (res_1[i].count != res_2.count) {
			printf(RED "Not the same number of neighbours for iteration %zu (point %zu): %zu - "
				   "%zu\n" COLOR_RESET,
			       (size_t)i, (size_t)indices[i], (size_t)res_1[i].count, (size_t)res_2.count);
		}

		// Check for same neighbours
		for (index_t j = 0; j < res_2.count; j++) {
			if (res_1[i].indices[j] != res_2.indices[j]) {

				const data_t epsilon = (data_t)1e-5;
				data_t diff = (data_t)fabs(res_1[i].distances[j] - res_2.distances[j]);

				if (diff > epsilon) {
					printf(RED "%zu (%f) - %zu (%f) | diff = %f\n" COLOR_RESET,
					       (size_t)res_1[i].indices[j], (double)res_1[i].distances[j],
					       (size_t)res_2.indices[j], (double)res_2.distances[j], (double)diff);
					exit(-1);
				}
			}
		}

		destroy_radius_result(&res_2);
	}
}

void test(const Points *points)
{
	srand((unsigned)time(NULL));

	// Reorder points
	Octree octree = {};
	create_octree(&octree, points);
	Points_TFG points_sorted = {};
	Solution *sol = reorder_cuthill_mckee(&octree);
	build_sorted_points(&points_sorted, octree.points, sol);
	destroy_solution(sol);
	destroy_octree(&octree);

	index_t indices[NUM_OF_TESTS] = {};
	for (int i = 0; i < NUM_OF_TESTS; ++i) {
		indices[i] = (index_t)rand() % points->num_points;
	}

	RadiusResult res_1[NUM_OF_TESTS] = {};
#pragma omp parallel for
	for (int i = 0; i < NUM_OF_TESTS; ++i) {
		find_radius_neighbors(points_sorted.points, indices[i], &(res_1[i]));
		sort_neighbors(res_1[i].indices, res_1[i].distances, res_1[i].count);
	}

	printf(MAGENTA "\nTesting Octree... " COLOR_RESET);
	test_points_for_octree(points_sorted.points, indices, res_1);
	printf(GREEN "PASSED\n" COLOR_RESET);

	printf(MAGENTA "Testing TFG... " COLOR_RESET);
	test_points_for_tfg(&points_sorted, indices, res_1);
	printf(GREEN "PASSED\n" COLOR_RESET);

	for (int i = 0; i < NUM_OF_TESTS; ++i) {
		destroy_radius_result(&(res_1[i]));
	}
	destroy_points_sorted(&points_sorted);
}