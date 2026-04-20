#pragma once
#include <stddef.h>

typedef struct{
    size_t row_size;
    size_t *row;
} index_row;

size_t get_index_row_size(index_row *row);

size_t estimate_index_row_size(size_t neighbours_size);

index_row *create_index_row(const size_t *neighbours, size_t neighbours_size);

void get_neighbours_index_row(const index_row *row, size_t *neighbours);

void destroy_index_row(index_row *row);