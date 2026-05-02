#pragma once
#include "../../points_structures/octree.h"
#include "../lidar_points.h"

#include "../../utils/auxiliar_structures/radius_result.h"
#include "row.h"

#define NUMBER_OF_BLOCKS 100

struct matrix_t {
	const Points *points;
	struct row_t **rows;
	size_t *bandwith;
};

void create_neighbourhood_matrix(struct matrix_t *matrix, const Octree *octree);

size_t get_block_index(size_t i, size_t num_points);

void get_neighbours_matrix(const struct matrix_t *matrix, size_t index, RadiusResult *result);

void destroy_neighbourhood_matrix(struct matrix_t *matrix);

size_t get_matrix_bandwidth(const struct matrix_t *matrix);

size_t get_max_num_elements_row(const struct matrix_t *matrix);

void print_matrix_stats(const struct matrix_t *matrix);