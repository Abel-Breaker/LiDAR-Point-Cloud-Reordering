#pragma once
#include "../points_structures/kd_tree.h"
#include "../points_structures/octree.h"

void check_neighborhoods_kd_tree(const KDTree *tree);

void check_neighborhoods_octree_knn(const Octree *octree);

void check_neighborhoods_octree_radius(const Octree *octree);