#pragma once
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix.h"

void check_neighborhoods_octree_knn(const Octree *octree);

void check_neighborhoods_octree_radius(const Octree *octree);

void check_neighborhoods_matrix_mix(const struct matrix_t *matrix);

void check_neighborhoods_tfg(const struct matrix_t *matrix);

void check_neighborhoods_tfg_opt(const struct matrix_t *matrix);