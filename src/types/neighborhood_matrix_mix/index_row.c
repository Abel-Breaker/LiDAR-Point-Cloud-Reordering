#include "index_row.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

size_t get_index_row_size(const index_row *row)
{
	return row->row_size + sizeof(index_row);
}



static size_t estimate_row_size(size_t neighbours_size)
{
	return neighbours_size * sizeof(size_t);
}

size_t estimate_index_row_size(size_t neighbours_size)
{
	return estimate_row_size(neighbours_size) + sizeof(index_row);
}

index_row *create_index_row(const size_t *neighbours, size_t neighbours_size)
{
	// Reserves memory
	index_row *row = malloc(sizeof(index_row));
	row->row_size = estimate_row_size(neighbours_size);
	row->row = malloc(row->row_size);

    memcpy(row->row, neighbours, row->row_size);

	for(size_t i=0; i<neighbours_size; ++i){
		//printf("%zu ", neighbours[i]);
	}
	//printf("\n");
    return row;
}

void get_neighbours_index_row(const index_row *row, size_t *neighbours){
    memcpy(neighbours, row->row, row->row_size);
}

void destroy_index_row(index_row *row){
    free(row->row);
    free(row);
}