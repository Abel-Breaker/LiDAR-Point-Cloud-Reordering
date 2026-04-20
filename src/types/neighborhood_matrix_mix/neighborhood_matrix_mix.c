#include "neighborhood_matrix_mix.h"
#include "../../neighborhood_algorithms/knn/kd_tree.h"
#include "bit_row.h"
#include <stdlib.h>
#include <stdio.h>


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

void create_neighbourhood_matrix_mix(matrix_mix *matrix, KDTree *tree)
{
	// Reserves memory
	matrix->points = tree->pts;
	matrix->rows = malloc(sizeof(void *) * matrix->points->num_points);
	matrix->row_type = malloc(sizeof(*matrix->row_type) * matrix->points->num_points);

	// Set matrix values
	for (size_t i = 0; i < tree->pts->num_points; ++i) {
		size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, i, neighbours, neighbours_distances);

		matrix->rows[i] = create_bit_row(find_min(neighbours, K), find_max(neighbours, K), neighbours);
	}
}

void get_neighbours_matrix_mix(matrix_mix *matrix, size_t index, size_t *neighbours)
{
    get_neighbours_bit_row(matrix->rows[index], neighbours);
}

void destroy_neighbourhood_matrix_mix(matrix_mix *matrix)
{
	free(matrix->row_type);
	for (size_t i = 0; i < matrix->points->num_points; ++i) {
		destroy_bit_row(matrix->rows[i]);
	}
	free(matrix->rows);
}

void print_matrix_mix_stats(matrix_mix *matrix){
    size_t total=0;
    for(size_t i=0; i<matrix->points->num_points; ++i){
        total += get_bit_row_size(matrix->rows[i]);
    }
    
    printf("=== Matrix Stats ===\n");
    printf("Estimated size:     %zu bytes (%.6f GB)\n", total, (double)total / (1024.0 * 1024.0 * 1024.0));
	printf("Best posible size:  %zu bytes (%.6f GB)\n", K*8*matrix->points->num_points , K*8*matrix->points->num_points/ (1024.0 * 1024.0 * 1024.0));
    printf("====================\n");
}