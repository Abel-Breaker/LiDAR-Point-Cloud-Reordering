#include "bit_row.h"
#include <stdlib.h>
#include <string.h>
#include "../../utils/parse_args.h"



static inline size_t get_row_size(size_t min_index, size_t max_index){
    return ((max_index - min_index + 1 + 7) / 8); // TODO: Change 8 to avx size vector
}

size_t get_bit_row_size(bit_row *row){
    return row->row_size + sizeof(bit_row);
}

size_t estimate_bit_row_size(size_t min_index, size_t max_index){
    return get_row_size(min_index, max_index) + sizeof(bit_row);
}

bit_row *create_bit_row(size_t min_index, size_t max_index, const size_t *neighbours){

    bit_row *row = malloc(sizeof(bit_row));
    // Reserves memory
    row->offset = min_index;
    row->row_size = get_row_size(min_index, max_index);
    row->row = malloc(row->row_size);

    // Load data
    memset(row->row, 0, row->row_size);
    for (size_t i = 0; i < (K); ++i) { // TODO: Esa resta ns yo
        size_t bit = neighbours[i] - min_index;
        size_t byte_index = bit / 8;
        size_t bit_index  = bit % 8;

        if (byte_index < row->row_size) {
            row->row[byte_index] |= (uint8_t)(1 << bit_index);
        }
    }
    return row;
}

// TODO: Optimice
/*void get_neighbours_bit_row(const bit_row *row, size_t *neighbours)
{
    size_t count = 0;

    for (size_t i = 0; i < row->row_size * 8; i++) {
        size_t byte_i = i / 8;
        size_t bit_i  = i % 8;
        if (row->row[byte_i] & (1u << bit_i))
            neighbours[(count)++] = i + row->offset;
    }
}*/

void get_neighbours_bit_row(const bit_row *row, size_t *neighbours)
{
    size_t count = 0;

    for (size_t i = 0; i < row->row_size * 8; i++) {
        size_t byte_i = i / 8;
        size_t bit_i  = i % 8;
        if (row->row[byte_i] & (1u << bit_i))
            neighbours[(count)++] = i + row->offset;
    }
}


void destroy_bit_row(bit_row *row){
    row->offset = 0;
    row->row_size = 0;
    free(row->row);
    free(row);
}