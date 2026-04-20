#pragma once
#include <stddef.h>
#include <stdint.h>

// TODO, el primer bit se puede ahorrar pq ya es el offset
typedef struct {
    size_t offset; // First index neighbour
    size_t row_size; // Size of row in bytes
    uint8_t *row; // Row of bytes (each byte has 8 index)
}bit_row;

size_t get_bit_row_size(bit_row *row);

size_t estimate_bit_row_size(size_t min_index, size_t max_index);

bit_row *create_bit_row(size_t min_index, size_t max_index, const size_t *neighbours);

void get_neighbours_bit_row(const bit_row *row, size_t *neighbours);

void destroy_bit_row(bit_row *row);