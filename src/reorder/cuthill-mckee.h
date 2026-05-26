#pragma once
#include "../octree/octree.h"
#include "../points/points.h"
#ifdef _OPENMP
#define NUM_PARALLEL_RUNS 7
#else
#define NUM_PARALLEL_RUNS 1

#endif
typedef struct {
	index_t *permutations;
	index_t *bandwith_left;
	index_t *bandwith_right;
	index_t total_neighours;
} Solution;

/**
 * @brief Reorder points and search the best solution with the lowest bandwith
 *
 * @param[in] octree
 * @return Pointer to solution
 *
 * @note Caller must free memory of solution with destroy_solution()
 */
Solution *reorder_cuthill_mckee(const Octree *octree);

/**
 * @brief Print stats of the solution founded: total/avg neighbours and max/avg bandwidth
 *
 * @param[in] solution
 * @param[in] num_points
 */
void print_solution_stats(const Solution *solution, index_t num_points);

/**
 * @brief Free memory for solution
 *
 * @param[in] octree
 * @return Pointer to solution
 *
 * @note Caller must free memory of solution with destroy_solution()
 */
void destroy_solution(Solution *solution);