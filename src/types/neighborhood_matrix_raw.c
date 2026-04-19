#include "neighborhood_matrix_raw.h"
#include "../neighborhood_algorithms/knn/kd_tree.h"
#include "../utils/parse_args.h"
#include <math.h>
//#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void create_neighborhood_matrix_raw(neighborhood_matrix_raw *matrix, const KDTree *tree)
{
	(*matrix) = malloc(sizeof(neighbours_row_raw) * tree->pts->num_points);

	// Test neighborhood
    #pragma omp parallel for
	for (size_t i = 0; i < tree->pts->num_points; ++i) {
		(*matrix)[i].row_size = K;
        (*matrix)[i].row = malloc(K * sizeof(size_t));
		double neighbours_distances[K];
		start_kdtree_knearest(tree, i, (*matrix)[i].row , neighbours_distances);
	}
    //printf("LLEGUE?\n");
}

void get_neighbours_matrix_raw(neighborhood_matrix_raw matrix, size_t index,
                    size_t *neighbours)
{
    memcpy(neighbours, matrix[index].row, matrix[index].row_size * sizeof(size_t));
}

void destroy_neighborhood_matrix_raw(neighborhood_matrix_raw matrix, size_t num_points)
{
    for(size_t i=0; i<num_points; ++i){
        free(matrix[i].row);
    }
    free(matrix);

}

void print_matrix_stats_raw(neighborhood_matrix_raw matrix, size_t num_points){
    if (!matrix || num_points == 0) {
        printf("Matrix is empty or NULL.\n");
        return;
    }

    size_t total_bytes = 0;

    for (size_t i = 0; i < num_points; ++i) {
        total_bytes += matrix[i].row_size * sizeof(size_t);
    }

    // Overhead de los structs
    size_t struct_overhead = num_points * sizeof(neighbours_row_raw);
    size_t total_estimated = total_bytes + struct_overhead;

    double total_gb = (double)total_estimated / (1024.0 * 1024.0 * 1024.0);

    printf("=== Matrix Stats ===\n");
    printf("Num points:         %zu\n", num_points);
    printf("Estimated size:     %zu bytes (%.6f GB)\n", total_estimated, total_gb);
    printf("====================\n");
}