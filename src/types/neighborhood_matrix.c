#include "neighborhood_matrix.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../utils/parse_args.h"
#include <math.h>
//#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static size_t find_min(const size_t neighbours[], size_t size) {
    size_t min = neighbours[0];
    for (size_t i = 1; i < size; i++) {
        if (neighbours[i] < min) {
            min = neighbours[i];
        }
    }
    return min;
}

static size_t find_max(const size_t neighbours[], size_t size) {
    size_t max = neighbours[0];
    for (size_t i = 1; i < size; i++) {
        if (neighbours[i] > max) {
            max = neighbours[i];
        }
    }
    return max;
}

static void set_neighbours(uint8_t *row, size_t row_size, size_t offset,
                           const size_t *neighbours, size_t neighbours_count)
{
    memset(row, 0, row_size);

    for (size_t i = 0; i < neighbours_count; ++i) {
        size_t bit = neighbours[i] - offset;
        size_t byte_index = bit / 8;
        size_t bit_index  = bit % 8;

        if (byte_index < row_size) {
            row[byte_index] |= (uint8_t)(1 << bit_index);
        }
    }
}

void create_neighborhood_matrix(neighborhood_matrix *matrix, const KDTree *tree)
{
	(*matrix) = malloc(sizeof(neighbours_row) * tree->pts->num_points);

	// Test neighborhood
    #pragma omp parallel for
	for (size_t i = 0; i < tree->pts->num_points; ++i) {

		size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, i, neighbours, neighbours_distances);

        size_t max = find_max(neighbours, K);
        size_t min = find_min(neighbours, K);
		size_t distance = max - min + 1;
        size_t num_bits = ((distance + 8 - 1) / 8) * 8; // TODO: Change 8 to avx size vector
        size_t num_bytes = num_bits/8;

        //printf("%ld -> Neighbours: %ld - %ld: distance: %ld - (%ld bytes)\n", i, max, min, distance, num_bytes);

        (*matrix)[i].offset = min;
        (*matrix)[i].row_size = num_bytes;
        (*matrix)[i].row = malloc(num_bytes);

        set_neighbours((*matrix)[i].row, num_bytes, min, neighbours, K);
        /*if(i<12){
            for(size_t j=0; j<K; ++j){
                printf("%ld ", neighbours[j]);
            }
            printf("\n");
            get_neighbours(*matrix, i, neighbours);
            for(size_t j=0; j<K; ++j){
                printf("%ld ", neighbours[j]);
            }
            printf("\n\n");
        }*/
	}
    //printf("LLEGUE?\n");
}

void get_neighbours(neighborhood_matrix matrix, size_t index,
                    size_t *neighbours)
{
    size_t count = 0;
    const neighbours_row *r = &matrix[index];

    for (size_t i = 0; i < r->row_size * 8; i++) {
        size_t byte_i = i / 8;
        size_t bit_i  = i % 8;
        if (r->row[byte_i] & (1u << bit_i))
            neighbours[(count)++] = i + r->offset;
    }
}

void destroy_neighborhood_matrix(neighborhood_matrix matrix, size_t num_points)
{
    for(size_t i=0; i<num_points; ++i){
        free(matrix[i].row);
    }
    free(matrix);

}

void print_matrix_stats(neighborhood_matrix matrix, size_t num_points){
    if (!matrix || num_points == 0) {
        printf("Matrix is empty or NULL.\n");
        return;
    }

    size_t total_bytes = 0;
    size_t min_row_size = matrix[0].row_size;
    size_t max_row_size = matrix[0].row_size;
    double sum_row_size = 0.0;

    for (size_t i = 0; i < num_points; ++i) {
        size_t row_bytes = matrix[i].row_size * sizeof(uint8_t);
        total_bytes += row_bytes;

        if (matrix[i].row_size < min_row_size) min_row_size = matrix[i].row_size;
        if (matrix[i].row_size > max_row_size) max_row_size = matrix[i].row_size;
        sum_row_size += (double)matrix[i].row_size;
    }

    // Overhead de los structs
    size_t struct_overhead = num_points * sizeof(neighbours_row);
    size_t total_estimated = total_bytes + struct_overhead;

    double avg_row_size = sum_row_size / (double)num_points;
    double total_gb = (double)total_estimated / (1024.0 * 1024.0 * 1024.0);

    printf("=== Matrix Stats ===\n");
    printf("Num points:         %zu\n", num_points);
    printf("Estimated size:     %zu bytes (%.6f GB)\n", total_estimated, total_gb);
    printf("Best posible size:  %zu bytes (%.6f GB)\n", K*8*num_points , K*8*num_points/ (1024.0 * 1024.0 * 1024.0));
    printf("Row size  min:      %zu\n", min_row_size);
    printf("Row size  max:      %zu\n", max_row_size);
    printf("Row size  avg:      %.2f\n", avg_row_size);
    printf("====================\n");
}