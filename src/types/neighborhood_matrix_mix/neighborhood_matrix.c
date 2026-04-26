#include "neighborhood_matrix.h"
#include "../../neighborhood_algorithms/radius_search/octree.h"
#include "row.h"
#include <limits.h> // for SIZE_MAX
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void create_neighbourhood_matrix(struct matrix_t *matrix, const Octree *octree)
{
	// Reserves memory
	matrix->points = octree->pts;
	matrix->rows = malloc(sizeof(*matrix->rows) * matrix->points->num_points);

// Calculates neighbors and set matrix values
#pragma omp parallel for
	for (size_t i = 0; i < octree->pts->num_points; ++i) {
		RadiusResultOctree res = {};
		octree_radius_search(octree, i, get_args()->radius_search, &res);

		matrix->rows[i] = create_row(res.indices, res.count);

		radius_result_destroy(&res);
	}
}

void get_neighbours_matrix(const struct matrix_t *matrix, size_t index, RadiusResult *result)
{
	const size_t *neighbors = get_neighbours_row(matrix->rows[index]);
	size_t num_elements = get_num_elements_row(matrix->rows[index]);

	reserves_memory_radius_result(result, num_elements);
	for (size_t i = 0; i < num_elements; i++) {
		result->indices[i] = neighbors[i];
		result->distances[i] = euclidian_distance_3d(
		    matrix->points->x[neighbors[i]], matrix->points->y[neighbors[i]], matrix->points->z[neighbors[i]],
		    matrix->points->x[index], matrix->points->y[index], matrix->points->z[index]);
	}
}

void destroy_neighbourhood_matrix(struct matrix_t *matrix)
{
	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		destroy_row(matrix->rows[i]);
	}
	free(matrix->rows);
}

void print_matrix_stats(const struct matrix_t *matrix)
{
	size_t n_rows = matrix->points->num_points;

	size_t total_elements = 0;
	size_t total_size = 0;

	size_t max_row = 0;
	size_t min_row = SIZE_MAX;

	for (size_t i = 0; i < n_rows; i++) {

		const struct row_t *row = matrix->rows[i];
		if (!row)
			continue;

		size_t elems = get_num_elements_row(row);

		total_elements += elems;

		if (elems > max_row) {
			max_row = elems;
		}
		if (elems < min_row) {
			min_row = elems;
		}

		total_size += get_row_t_size(row);
	}

	double avg = (double)total_elements / (double)n_rows;
	double total_gb = (double)total_size / (1024.0 * 1024.0 * 1024.0);

	printf("Matrix stats:\n");
	printf("  Total rows: %zu\n", n_rows);
	printf("  Total elements: %zu\n", total_elements);
	printf("  Average elements per row: %.2f\n", avg);
	printf("  Max elements in row: %zu\n", max_row);
	printf("  Min elements in row: %zu\n", min_row);
	printf("Total size: %zu bytes (%.6f GB)\n", total_size, total_gb);
}