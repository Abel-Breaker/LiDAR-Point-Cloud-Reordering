#pragma once
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix_mix.h"

void neighborhoods_kd_tree_knn_bench(const KDTree *structure);

void neighborhoods_octree_knn_bench(const Octree *structure);

void neighborhoods_octree_radius_bench(const Octree *structure);

void neighborhoods_matrix_mix_bench(const matrix_mix *matrix);

void neighborhoods_kdtree_prune_knn_bench(const KDTreePrune *structure);