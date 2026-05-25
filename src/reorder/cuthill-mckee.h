#pragma once
#include "../points/points.h"
#include "../octree/octree.h"
#ifdef _OPENMP
#define NUM_PARALLEL_RUNS 7
#else
#define NUM_PARALLEL_RUNS 1

#endif
typedef struct {
	size_t *permutations;
	size_t *bandwith_left;
	size_t *bandwith_right;
} Solution;

/**
 * @brief Reorder points.
 *
 * @param[in] octree
 * @param[out] new_points
 * 
 * @note Not thread-safe.
 */
Solution *reorder_cuthill_mckee(const Octree *octree);

void destroy_solution(Solution *solution);