#include "neighborhood_matrix_mix.h"
#include "../../neighborhood_algorithms/knn/kd_tree.h"
#include "bit_row.h"
#include "index_row.h"
#include <stdio.h>
#include <stdlib.h>

static size_t find_min(const size_t neighbours[], size_t size)
{
	size_t min = neighbours[0];
	for (size_t i = 1; i < size; i++) {
		if (neighbours[i] < min) {
			min = neighbours[i];
		}
	}
	return min;
}

static size_t find_max(const size_t neighbours[], size_t size)
{
	size_t max = neighbours[0];
	for (size_t i = 1; i < size; i++) {
		if (neighbours[i] > max) {
			max = neighbours[i];
		}
	}
	return max;
}

void create_neighbourhood_matrix_mix(matrix_mix *matrix, const KDTree *tree)
{
	// Reserves memory
	matrix->points = tree->pts;
	matrix->rows = malloc(sizeof(void *) * matrix->points->num_points);
	matrix->row_type = malloc(sizeof(*matrix->row_type) * matrix->points->num_points);

// Set matrix values
#pragma omp parallel for
	for (size_t i = 0; i < tree->pts->num_points; ++i) {
		size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, i, neighbours, neighbours_distances);

		size_t min = find_min(neighbours, K);
		size_t max = find_max(neighbours, K);
		size_t bit_row_size = estimate_bit_row_size(min, max);
		size_t index_row_size = estimate_index_row_size(K);

		if (bit_row_size < index_row_size) {
			matrix->rows[i] = create_bit_row(min, max, neighbours);
			matrix->row_type[i] = BIT_ROW;
		} else {
			matrix->rows[i] = create_index_row(neighbours, K);
			matrix->row_type[i] = INDEX_ROW;
		}
		/*if (i < 2) {
			for (size_t j = 0; j < K; ++j) {
				printf("%ld ", neighbours[j]);
			}
			printf("\n");
		}*/
	}
}

void get_neighbours_matrix_mix(const matrix_mix *matrix, size_t index, size_t *neighbours)
{
	if (matrix->row_type[index] == BIT_ROW) {
		get_neighbours_bit_row(matrix->rows[index], neighbours);
	} else {
		get_neighbours_index_row(matrix->rows[index], neighbours);
	}
}

void destroy_neighbourhood_matrix_mix(matrix_mix *matrix)
{
	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		if (matrix->row_type[i] == BIT_ROW) {
			destroy_bit_row(matrix->rows[i]);
		} else {
			destroy_index_row(matrix->rows[i]);
		}
	}
	free(matrix->row_type);
	free(matrix->rows);
}

#include <limits.h> // para SIZE_MAX

void print_matrix_mix_stats(const matrix_mix *matrix)
{
	size_t total = 0, bit_row_count = 0, index_row_count = 0;

	size_t min_bit_row = SIZE_MAX;
	size_t max_bit_row = 0;
	size_t sum_bit_row = 0;

	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		if (matrix->row_type[i] == BIT_ROW) {
			size_t size = get_bit_row_size(matrix->rows[i]);

			total += size;
			sum_bit_row += size;

			if (size < min_bit_row)
				min_bit_row = size;
			if (size > max_bit_row)
				max_bit_row = size;

			bit_row_count++;
		} else {
			total += get_index_row_size(matrix->rows[i]);
			index_row_count++;
		}
	}

	double avg_bit_row = 0.0;
	if (bit_row_count > 0) {
		avg_bit_row = (double)sum_bit_row / (double)bit_row_count;
	} else {
		min_bit_row = 0; // evitar imprimir SIZE_MAX si no hay BIT_ROW
	}

	printf("=== Matrix Stats ===\n");
	printf("Estimated size:     %zu bytes (%.6f GB)\n", total, (double)total / (1024.0 * 1024.0 * 1024.0));

	printf("Best posible size:  %zu bytes (%.6f GB)\n", (K * 8 + sizeof(index_row)) * matrix->points->num_points,
	       (double)((K * 8 + sizeof(index_row)) * matrix->points->num_points) / (1024.0 * 1024.0 * 1024.0));

	printf("Bit row count:      %zu\n", bit_row_count);
	printf("Index row count:    %zu\n", index_row_count);
	printf("Bit Row min size:   %zu bytes\n", min_bit_row);
	printf("Bit Row max size:   %zu bytes\n", max_bit_row);
	printf("Bit Row avg size:   %.2f bytes\n", avg_bit_row);

	printf("====================\n");
}