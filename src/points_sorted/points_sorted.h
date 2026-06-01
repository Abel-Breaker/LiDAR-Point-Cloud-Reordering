#pragma once
#include "../../utils/auxiliar_structures/radius_result.h"
#include "../../utils/types.h"
#include "../points/points.h"
#include "../reorder/cuthill-mckee.h"

typedef struct {
	Points *points;
	index_t *bandwith_left;
	index_t *bandwith_right;
	index_t max_bandwith;
} Points_sorted;

/**
 * @brief Calculate bandwith_left and bandwith_right to do optimized search.
 *
 * @param[in,out] points Pointer to the Points_sorted structure.
 * @param[in,out] old_points Pointer to Points structure with old points and order.
 * @param[in,out] solution Pointer to the Solution structure with the permutations and bandwith.
 *
 * @note It must called be after add all points and before do any search
 */
void build_sorted_points(Points_sorted *points, const Points *old_points, const Solution *solution);

/**
 * @brief Get the block index of a point in the bandwith array.
 *
 * @param[in] i Index of the pointin the cloud to obtain block index.
 * @param[in] num_points Number of points of the point cloud.
 *
 * @return Block index of the point in the bandwith array.
 */
index_t get_block_index(index_t i, index_t num_points);

/**
 * @brief Search neighbours of a point in a radius.
 *
 * @param[in] points Points_sorted struct pointer where search neighbours.
 * @param[in] index Index of the point on which to search for neighbors.
 * @param[in] result Pointer to RadiusResult where solution will be saved.
 *
 * @note Before calling this function is needed to call `build_sorted_points`.
 */
void tfg_radius_search(const Points_sorted *points, index_t index, RadiusResult *result);

/**
 * @brief Frees the memory allocated for Points_sorted.
 *
 * @param[in,out] points Pointer to the Points structure to destroy.
 */
void destroy_points_sorted(Points_sorted *points);
