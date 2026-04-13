#pragma once
#include "../points_structures/kd_tree.h"
#include "../points_structures/kd_tree_prune.h"
#include "../points_structures/octree.h"

typedef void (*NeighborFunc)(const void *structure, size_t point_index, size_t *neighbours_index, double *neighbours_distances);

void neighborhoods_kd_tree_knn_bench(const KDTree *structure);

void neighborhoods_octree_knn_bench(const Octree *structure);

void neighborhoods_octree_radius_bench(const Octree *structure);

void neighborhoods_kdtree_prune_knn_bench(const KDTreePrune *structure);