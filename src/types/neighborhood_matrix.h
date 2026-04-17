#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../points_structures/kd_tree.h"

typedef struct{
    size_t offset;
    size_t row_size;
    uint8_t *row;
} neighbours_row;

typedef neighbours_row* neighborhood_matrix;

void create_neighborhood_matrix(neighborhood_matrix *matrix, const KDTree *tree);

void get_neighbours(neighborhood_matrix matrix, size_t index, size_t *neighbours);

void destroy_neighborhood_matrix(neighborhood_matrix matrix, size_t num_points);