#include "neighborhood_matrix.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../utils/parse_args.h"
#include <math.h>
//#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

size_t find_min(const size_t neighbours[], size_t size) {
    size_t min = neighbours[0];
    for (size_t i = 1; i < size; i++) {
        if (neighbours[i] < min) {
            min = neighbours[i];
        }
    }
    return min;
}

size_t find_max(const size_t neighbours[], size_t size) {
    size_t max = neighbours[0];
    for (size_t i = 1; i < size; i++) {
        if (neighbours[i] > max) {
            max = neighbours[i];
        }
    }
    return max;
}

static void set_neighbours(uint8_t *row, size_t row_size,
                           const size_t *neighbours, size_t neighbours_count)
{
    memset(row, 0, row_size);

    for (size_t i = 0; i < neighbours_count; ++i) {
        size_t bit = neighbours[i];
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
	for (size_t i = 0; i < tree->pts->num_points; ++i) {

		size_t neighbours[K];
		double neighbours_distances[K];
		start_kdtree_knearest(tree, i, neighbours, neighbours_distances);

        size_t max = find_max(neighbours, K);
        size_t min = find_min(neighbours, K);
		size_t distance = max - min;
        size_t num_bits = ((distance + 8 - 1) / 8) * 8; // TODO: Change 8 to avx size vector
        size_t num_bytes = num_bits/8;

        //printf("%ld -> Neighbours: %ld - %ld: distance: %ld - (%ld bytes)\n", i, max, min, distance, num_bytes);

        (*matrix)[i].offset = neighbours[0];
        (*matrix)[i].row_size = num_bytes;
        (*matrix)[i].row = malloc(num_bytes);

        set_neighbours((*matrix)[i].row, num_bytes, neighbours, K);
	}
    //printf("LLEGUE?\n");
}

void get_neighbours(neighborhood_matrix matrix, size_t index,
                    size_t *neighbours)
{
    size_t count = 0;
    const neighbours_row *r = &matrix[index];

    for (size_t i = r->offset; i < r->row_size * 8; i++) {
        size_t byte_i = i / 8;
        size_t bit_i  = i % 8;
        if (r->row[byte_i] & (1u << bit_i))
            neighbours[(count)++] = i;
    }
}

void destroy_neighborhood_matrix(neighborhood_matrix matrix, size_t num_points)
{
    for(size_t i=0; i<num_points; ++i){
        free(matrix[i].row);
        
    }
    free(matrix);

}