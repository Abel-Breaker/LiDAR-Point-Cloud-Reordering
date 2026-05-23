#pragma once
#include "../points/points.h"
#include "../octree/octree.h"


/**
 * @brief Reorder points.
 *
 * @param[in] octree
 * @param[out] new_points
 * 
 * @note Not thread-safe.
 */
void reorder_cuthill_mckee(const Octree *octree, size_t *permutations);