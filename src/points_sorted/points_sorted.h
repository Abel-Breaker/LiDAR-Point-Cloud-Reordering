#pragma once
#include "../points/points.h"
#include "../reorder/cuthill-mckee.h"
#include "../../utils/auxiliar_structures/radius_result.h"
#include "../../utils/types.h"
#include <math.h>




typedef struct{
    Points *points;
    index_t *bandwith_left;
	index_t *bandwith_right;
} Points_TFG;


index_t get_block_index(index_t i, index_t num_points);

/**
 * Calculate bandwith_left and bandwith_right to do optimized search.
 *
 * @param[in,out] points Pointer to the Points structure.
 * 
 * @note It must called be after add all points and before do any search
 */
void build_sorted_points(Points_TFG *points, const Points *old_points, const Solution *solution);


/**
 * Frees the memory allocated for a set of points.
 *
 * @param[in,out] points Pointer to the Points structure to destroy.
 */
void destroy_points_sorted(Points_TFG *points);

void tfg_radius_search(const Points_TFG *points, index_t index, RadiusResult *result);
