#pragma once
#include "../../types/lidar_points.h"
#include "../../utils/parse_args.h"

/**
 * Finds the K nearest neighbors of a point in a point collection.
 *
 * @param[in] points Pointer to the collection of points.
 * @param[in] point_index Index of the point in the collection for which neighbors are being searched.
 * @param[out] neighbours_index Array of size K where the indices of the nearest neighbors will be stored.
 * @param[out] neighbours_distances Array of size K where the distances to the nearest neighbors will be stored.
 * 
 * @note The point collection `points` must be initialized before calling this function.
 */
void find_point_neighbors(const Points *points, size_t point_index, size_t neighbours_index[K], double neighbours_distances[K]);