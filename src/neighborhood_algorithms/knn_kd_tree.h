#pragma once
#include "../structures/kd_tree.h"

#define K 75


void start_kdtree_knearest(KDTree *tree, size_t point_index, size_t neighbours_index[K], double neighbours_distances[K]);