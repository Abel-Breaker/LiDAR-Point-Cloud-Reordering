#pragma once
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix.h"

void neighborhoods_octree_knn_bench(const Octree *structure);

void neighborhoods_octree_radius_bench(const Octree *structure);

void neighborhoods_matrix_bench(const struct matrix_t *matrix);

void neighborhoods_tfg_bench(const struct matrix_t *matrix);