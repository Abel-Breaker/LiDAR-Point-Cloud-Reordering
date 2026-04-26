#include "row.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static inline size_t calculate_indices_size(size_t num_elements)
{
	return num_elements * sizeof(size_t);
}

size_t get_row_t_size(const struct row_t *row)
{
	return calculate_indices_size(row->num_elements) + sizeof(*row);
}


struct row_t *create_row(const size_t *neighbour_indices, size_t indices_size)
{
	// Reserves memory
	struct row_t *row = malloc(sizeof(*row));
	row->num_elements = indices_size;
	row->indices = malloc(calculate_indices_size(indices_size));

    memcpy(row->indices, neighbour_indices, calculate_indices_size(indices_size));

	for(size_t i=0; i<indices_size; ++i){
		//printf("%zu ", neighbours[i]);
	}
	//printf("\n");
    return row;
}

const size_t *get_neighbours_row(const struct row_t *row){
    return row->indices;
}

size_t get_num_elements_row(const struct row_t *row){
	return row->num_elements;
}

void destroy_row(struct row_t *row){
    free(row->indices);
    free(row);
}