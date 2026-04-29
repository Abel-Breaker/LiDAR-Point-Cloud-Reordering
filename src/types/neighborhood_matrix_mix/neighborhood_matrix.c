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

size_t get_max_num_elements_row(const struct matrix_t *matrix){
	size_t max_row = 0;
	for (size_t i = 0; i <  matrix->points->num_points; i++) {
		if(max_row < matrix->rows[i]->num_elements){
			max_row = matrix->rows[i]->num_elements;
		}
	}
	return max_row;
}

size_t get_matrix_bandwidth(const struct matrix_t *matrix){
	size_t n_rows = matrix->points->num_points;
	size_t max_index_range = 0;

	for (size_t i = 0; i < n_rows; i++) {

		const struct row_t *row = matrix->rows[i];
		if (!row)
			continue;

		// Bandwith
		if (row->num_elements > 0 && row->indices) {

			size_t min_idx = row->indices[0];
			size_t max_idx = row->indices[0];

			for (size_t j = 1; j < row->num_elements; j++) {
				size_t v = row->indices[j];
				if (v < min_idx) min_idx = v;
				if (v > max_idx) max_idx = v;
			}

			size_t range = max_idx - min_idx;

			if (range > max_index_range)
				max_index_range = range;
		}
	}
	return max_index_range;
}

void print_matrix_stats(const struct matrix_t *matrix)
{
	size_t n_rows = matrix->points->num_points;

	size_t total_elements = 0;
	size_t total_size = 0;

	size_t max_row = 0;
	size_t min_row = SIZE_MAX;

	// NUEVO: stats de rangos de índices
	size_t max_index_range = 0;
	size_t min_index_range = SIZE_MAX;
	size_t total_index_range = 0;
	size_t valid_range_rows = 0;

	for (size_t i = 0; i < n_rows; i++) {

		const struct row_t *row = matrix->rows[i];
		if (!row)
			continue;

		size_t elems = row->num_elements;

		total_elements += elems;

		if (elems > max_row)
			max_row = elems;
		if (elems < min_row)
			min_row = elems;

		total_size += get_row_t_size(row);

		// Bandwith
		if (row->num_elements > 0 && row->indices) {

			size_t min_idx = row->indices[0];
			size_t max_idx = row->indices[0];

			for (size_t j = 1; j < row->num_elements; j++) {
				size_t v = row->indices[j];
				if (v < min_idx) min_idx = v;
				if (v > max_idx) max_idx = v;
			}

			size_t range = max_idx - min_idx;

			if (range > max_index_range)
				max_index_range = range;
			if (range < min_index_range)
				min_index_range = range;

			total_index_range += range;
			valid_range_rows++;
		}
	}

	double avg = (n_rows > 0)
		? (double)total_elements / (double)n_rows
		: 0.0;

	double avg_range = (valid_range_rows > 0)
		? (double)total_index_range / (double)valid_range_rows
		: 0.0;

	double total_gb = (double)total_size / (1024.0 * 1024.0 * 1024.0);

	printf("Matrix stats:\n");
	printf("  Total rows: %zu\n", n_rows);
	printf("  Total elements: %zu\n", total_elements);
	printf("  Average elements per row: %.2f\n", avg);
	printf("  Max elements in row: %zu\n", max_row);
	printf("  Min elements in row: %zu\n", min_row);

	printf("Index range stats per row:\n");
	printf("  Max index range: %zu\n", max_index_range);
	printf("  Min index range: %zu\n", min_index_range);
	printf("  Avg index range: %.2f\n", avg_range);

	printf("Total size: %zu bytes (%.6f GB)\n", total_size, total_gb);
}