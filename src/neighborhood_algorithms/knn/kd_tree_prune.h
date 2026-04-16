#pragma once
#include "../../points_structures/kd_tree_prune.h"
#include "../../utils/parse_args.h"

/**
 * Finds the K nearest neighbors of a point in a KD-tree.
 *
 * @param[in] tree Pointer to the KD-tree.
 * @param[in] point_index Index of the point for which neighbors are searched.
 * @param[out] neighbours_index Array of size K where neighbor indices will be stored.
 * @param[out] neighbours_distances Array of size K where distances to neighbors will be stored.
 *
 * @note The KD-tree `tree` must be initialized before calling this function.
 */
void start_kdtree_prune_knearest(const KDTreePrune *tree, size_t point_index, size_t neighbours_index[K], double neighbours_distances[K]);

void set_upper_bound_distance(KDTreePrune *tree);