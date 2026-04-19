#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../points_structures/kd_tree.h"


typedef struct{
    size_t row_size;
    size_t *row;
} neighbours_row_raw;

typedef neighbours_row_raw* neighborhood_matrix_raw;

void create_neighborhood_matrix_raw(neighborhood_matrix_raw *matrix, const KDTree *tree);

void get_neighbours_matrix_raw(neighborhood_matrix_raw matrix, size_t index, size_t *neighbours);

void destroy_neighborhood_matrix_raw(neighborhood_matrix_raw matrix, size_t num_points);

void print_matrix_stats_raw(neighborhood_matrix_raw matrix, size_t num_points);