#include "neighborhood_matrix.h"
#include "../../neighborhood_algorithms/radius_search/octree.h"
#include "row.h"
#include "../../utils/parse_args.h"
#include <limits.h> // for SIZE_MAX
#ifdef _OPENMP
#include <omp.h>
#else
static inline int omp_get_max_threads(void) { return 1; }
static inline int omp_get_thread_num(void) { return 0; }
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


size_t get_block_index(size_t i, size_t num_points)
{
	const size_t number_of_blocks = get_args()->number_of_blocks;

    if (num_points == 0) return 0;
    size_t block_size = (num_points + number_of_blocks - 1) / number_of_blocks; // ceil
    size_t block = i / block_size;
    return (block >= number_of_blocks) ? number_of_blocks - 1 : block;
}

void create_neighbourhood_matrix(struct matrix_t *matrix, const Octree *octree)
{
	const size_t n = octree->points->num_points;
	const size_t number_of_blocks = get_args()->number_of_blocks;

	matrix->points = octree->points;
	matrix->rows = malloc(n * sizeof(struct row_t *)); // sizeof(*(matrix->rows)) shows a Clan-Tidy false positive warn

	matrix->bandwith_left = calloc(number_of_blocks, sizeof(size_t));
	matrix->bandwith_right = calloc(number_of_blocks, sizeof(size_t));

	int max_threads = omp_get_max_threads();

	// Bandwidth local por thread y bloque
	size_t (*local_bw_left)[number_of_blocks] = calloc((size_t)max_threads, sizeof(*local_bw_left));
	size_t (*local_bw_right)[number_of_blocks] = calloc((size_t)max_threads, sizeof(*local_bw_right));

#pragma omp parallel
	{
		int tid = omp_get_thread_num();

#pragma omp for
		for (size_t i = 0; i < n; ++i) {

			RadiusResultOctree res = {};
			octree_radius_search(octree, i, get_args()->radius_search, &res);

			matrix->rows[i] = create_row(res.indices, res.count);

			if (res.count > 0 && res.indices) {

				size_t min_idx = i;
				size_t max_idx = i;

				// Buscar límites inferior y superior reales
				for (size_t j = 0; j < res.count; ++j) {
					size_t v = res.indices[j];

					if (v < min_idx) {
						min_idx = v;
					}

					if (v > max_idx) {
						max_idx = v;
					}
				}

				size_t bw_left = i - min_idx;
				size_t bw_right = max_idx - i;

				size_t block = get_block_index(i, n);

				if (bw_left > local_bw_left[tid][block]) {
					local_bw_left[tid][block] = bw_left;
				}

				if (bw_right > local_bw_right[tid][block]) {
					local_bw_right[tid][block] = bw_right;
				}
			}

			radius_result_destroy(&res);
		}
	}

	// Reducción final
	for (int t = 0; t < max_threads; ++t) {
		for (size_t b = 0; b < number_of_blocks; ++b) {

			if (local_bw_left[t][b] > matrix->bandwith_left[b]) {
				matrix->bandwith_left[b] = local_bw_left[t][b];
			}

			if (local_bw_right[t][b] > matrix->bandwith_right[b]) {
				matrix->bandwith_right[b] = local_bw_right[t][b];
			}
		}
	}

	free(local_bw_left);
	free(local_bw_right);
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
	free(matrix->bandwith_left);
	free(matrix->bandwith_right);
	free(matrix->rows);
}

size_t get_max_num_elements_row(const struct matrix_t *matrix)
{
	size_t max_row = 0;
	for (size_t i = 0; i < matrix->points->num_points; i++) {
		if (max_row < matrix->rows[i]->num_elements) {
			max_row = matrix->rows[i]->num_elements;
		}
	}
	return max_row;
}

size_t get_matrix_bandwidth(const struct matrix_t *matrix)
{
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
				if (v < min_idx)
					min_idx = v;
				if (v > max_idx)
					max_idx = v;
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

	// Stats de rangos de índices
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

		// Clasificación triangular basada en índices (i, j)
		if (row->num_elements > 0 && row->indices) {

			// Bandwidth (rango de índices en la fila)
			size_t min_idx = row->indices[0];
			size_t max_idx = row->indices[0];

			for (size_t j = 1; j < row->num_elements; j++) {
				size_t v = row->indices[j];
				if (v < min_idx)
					min_idx = v;
				if (v > max_idx)
					max_idx = v;
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

	double avg = (n_rows > 0) ? (double)total_elements / (double)n_rows : 0.0;
	double avg_range = (valid_range_rows > 0) ? (double)total_index_range / (double)valid_range_rows : 0.0;
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

	/*fprintf(stderr, "  BANDWIDTH LEFT: ");
	for (size_t i = 0; i < get_args()->number_of_blocks;; ++i) {
		//fprintf(stderr, "%zu ", matrix->bandwith_left[i]);
	}
	fprintf(stderr, "\n  BANDWIDTH RIGH: ");
	for (size_t i = 0; i < get_args()->number_of_blocks;; ++i) {
		//fprintf(stderr, "%zu ", matrix->bandwith_right[i]);
	}*/

	printf("\nTotal size: %zu bytes (%.6f GB)\n", total_size, total_gb);
}