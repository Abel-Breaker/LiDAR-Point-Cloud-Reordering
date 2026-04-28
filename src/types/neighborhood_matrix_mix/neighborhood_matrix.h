#pragma once
#include "../../points_structures/octree.h"
#include "../lidar_points.h"

#include "../../utils/auxiliar_structures/radius_result.h"
#include "row.h"

struct matrix_t {
	const Points *points;
	struct row_t **rows;
};

void create_neighbourhood_matrix(struct matrix_t *matrix, const Octree *octree);

void get_neighbours_matrix(const struct matrix_t *matrix, size_t index, RadiusResult *result);

void destroy_neighbourhood_matrix(struct matrix_t *matrix);

size_t get_max_num_elements_row(const struct matrix_t *matrix);

void print_matrix_stats(const struct matrix_t *matrix);