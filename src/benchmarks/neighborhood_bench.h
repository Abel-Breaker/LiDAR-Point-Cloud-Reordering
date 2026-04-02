#pragma once
#include "../points_structures/kd_tree.h"
#include "../points_structures/octree.h"

void neighborhoods_kd_tree_knn_bench(const KDTree *structure);

void neighborhoods_octree_knn_bench(const Octree *structure);

void neighborhoods_octree_radius_bench(const Octree *structure);