#pragma once
#include <stddef.h>

struct row_t{
    size_t num_elements;
    size_t *indices;
};

size_t get_row_t_size(const struct row_t *row);

struct row_t *create_row(const size_t *neighbour_indices, size_t indices_size);

const size_t *get_neighbours_row(const struct row_t *row);

size_t get_num_elements_row(const struct row_t *row);

void destroy_row(struct row_t *row);