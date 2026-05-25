#pragma once
#include <math.h>
#include <stddef.h>
#include "../../utils/auxiliar_structures/radius_result.h"
#include "../../utils/types.h"

#define DIMENSIONS 3

enum axis { X = 0, Y = 1, Z = 2 };

/**
 * @brief Computes the Euclidean distance between two points in 3D space.
 * 
 * @note It avoids sqrt for optimization
 */
static inline double euclidian_distance_3d(double x2, double y2, double z2,
                                           double x1, double y1, double z1)
{
	//return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
    return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
}

typedef struct{
    double *x;
    double *y;
    double *z;
    index_t num_points;
} Points;

/**
 * @brief Allocates memory for a set of points.
 *
 * @param[out] points Pointer to the Points structure to allocate.
 * @param[in] number_of_points Number of points to reserve space for.
 * @return true if memory allocation was successful, false otherwise.
 */
bool reserve_memory_points(Points *points, index_t number_of_points);

/**
 * @brief Adds a point to a set of points at a specified index.
 *
 * @param[in,out] points Pointer to the Points structure.
 * @param[in] index Index at which the point will be added.
 * @param[in] x, y, z Coordinates of the point to add.
 */
void add_point(Points *points, index_t index, double x, double y, double z);

/**
 * @brief Finds the neighbors of a point in a certain radio of a point collection.
 *
 * @param[in] points Pointer to the collection of points.
 * @param[in] point_index Index of the point in the collection for which neighbors are being searched.
 * @param[out] neighbours_index Array of size K where the indices of the nearest neighbors will be stored.
 * @param[out] neighbours_distances Array of size K where the distances to the nearest neighbors will be stored.
 * 
 * @note The point collection `points` must be initialized before calling this function.
 */
void find_radius_neighbors(const Points *points, index_t point_index, RadiusResult *result);

/**
 * @brief Frees the memory allocated for a set of points.
 *
 * @param[in,out] points Pointer to the Points structure to destroy.
 */
void destroy_points(Points *points);