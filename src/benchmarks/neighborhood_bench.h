#pragma once
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"
#include "../types/neighborhood_matrix.h"
#include "../types/neighborhood_matrix_raw.h"

void neighborhoods_kd_tree_knn_bench(const KDTree *structure);

void neighborhoods_octree_knn_bench(const Octree *structure);

void neighborhoods_octree_radius_bench(const Octree *structure);

void neighborhoods_matrix_bench(neighborhood_matrix matrix, const Points *points);

void neighborhoods_matrix_bench_raw(neighborhood_matrix_raw matrix, const Points *points);

void neighborhoods_kdtree_prune_knn_bench(const KDTreePrune *structure);