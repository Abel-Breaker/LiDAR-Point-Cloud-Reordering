#pragma once
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix.h"
#include "../types/neighborhood_matrix_raw.h"

void check_neighborhoods_kd_tree(const KDTree *tree);

void check_neighborhoods_kd_tree_prune(const KDTreePrune *tree);

void check_neighborhoods_octree_knn(const Octree *octree);

void check_neighborhoods_octree_radius(const Octree *octree);


void check_neighborhoods_matrix_raw(const neighborhood_matrix_raw matrix, const Points *pts);

void check_neighborhoods_matrix(const neighborhood_matrix matrix, const Points *pts);