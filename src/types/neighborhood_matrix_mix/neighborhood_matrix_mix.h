#pragma once
#include "../lidar_points.h"


typedef enum : bool { BIT_ROW, INDEX_ROW } TYPE_ROW;
#include "../../points_structures/kd_tree.h"

typedef struct{
	const Points *points;
	TYPE_ROW *row_type;
	void **rows;
}matrix_mix;

void create_neighbourhood_matrix_mix(matrix_mix *matrix, KDTree *tree);

void get_neighbours_matrix_mix(matrix_mix *matrix, size_t index, size_t *neighbours);

void destroy_neighbourhood_matrix_mix(matrix_mix *matrix);

void print_matrix_mix_stats(matrix_mix *matrix);