#pragma once
#include "../src/octree/octree.h"
#include "../src/points_sorted/points_sorted.h"

/**
 * @brief Runs the octree-based radius neighborhood benchmark.
 *
 * @param structure Pointer to the octree structure to evaluate.
 */
void neighborhoods_octree_radius_bench(const Octree *structure);

/**
 * @brief Runs the neighborhood benchmark using the TFG approach.
 *
 * @param points Pointer to the sorted points structure to evaluate.
 */
void neighborhoods_tfg_bench(const Points_sorted *points);