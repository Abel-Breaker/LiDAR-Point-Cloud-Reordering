#pragma once
#include "../lidar_points.h"
#include "../../points_structures/kd_tree.h"

typedef enum : bool { BIT_ROW, INDEX_ROW } TYPE_ROW;

typedef struct {
	const Points *points;
	TYPE_ROW *row_type;
	void **rows;
} matrix_mix;

void create_neighbourhood_matrix_mix(matrix_mix *matrix, const KDTree *tree);

void get_neighbours_matrix_mix(const matrix_mix *matrix, size_t index, size_t *neighbours);

void destroy_neighbourhood_matrix_mix(matrix_mix *matrix);

void print_matrix_mix_stats(const matrix_mix *matrix);