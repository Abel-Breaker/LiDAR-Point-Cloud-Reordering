#pragma once
#include "../points/points.h"
#include "../octree/octree.h"
#include "../../utils/auxiliar_structures/radius_result.h"
#include <math.h>
#include <stddef.h>

typedef struct{
    size_t num_neighbours;
    size_t max_neighbours; // For a unique point
    float avg_neighbours;
    size_t max_bandwith;
    float avg_bandwith;
} Points_sorted_stats;

typedef struct{
    Points *points;
    size_t *bandwith_left;
	size_t *bandwith_right;
    Points_sorted_stats stats;
} Points_TFG;


size_t get_block_index(size_t i, size_t num_points);

/**
 * Calculate bandwith_left and bandwith_right to do optimized search.
 *
 * @param[in,out] points Pointer to the Points structure.
 * 
 * @note It must called be after add all points and before do any search
 */
void build_sorted_points(Points_TFG *points, const Octree *octree, const size_t *permutations);


/**
 * Frees the memory allocated for a set of points.
 *
 * @param[in,out] points Pointer to the Points structure to destroy.
 */
void destroy_points_sorted(Points_TFG *points);

void print_points_sorted_stats(const Points_TFG *points);

void tfg_radius_search(const Points_TFG *points, size_t index, RadiusResult *result);
