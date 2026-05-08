#pragma once
#include "../types/lidar_points.h"
#include "../points_structures/octree.h"


/**
 * @brief Reorder points.
 *
 * @param[in] octree
 * @param[out] new_points
 * 
 * @note Not thread-safe.
 */
void reorder_cuthill_mckee(const Octree *octree, Points *new_points);