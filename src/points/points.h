#pragma once
#include "../../utils/auxiliar_structures/radius_result.h"
#include "../../utils/types.h"

#define DIMENSIONS 3

enum axis { X = 0, Y = 1, Z = 2 };

/**
 * @brief Computes the Euclidean distance between two points in 3D space.
 * 
 * @note It avoids sqrt for optimization
 */
static inline data_t euclidian_distance_3d(data_t x2, data_t y2, data_t z2,
                                           data_t x1, data_t y1, data_t z1)
{
    return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
}

typedef struct{
    data_t *x;
    data_t *y;
    data_t *z;
    index_t num_points;
} Points;

/**
 * @brief Allocates memory for a set of points.
 *
 * @param[out] points Pointer to the Points structure to allocate.
 * @param[in] number_of_points Number of points to reserve space for.
 * 
 * @return true if memory allocation was successful, false otherwise.
 */
bool reserve_memory_points(Points *points, index_t number_of_points);

/**
 * @brief Adds a point to a set of points at a specified index.
 *
 * @param[in,out] points Pointer to the Points structure.
 * @param[in] index Index at which the point will be added.
 * @param[in] x X coordinate of the point to add.
 * @param[in] y Y coordinate of the point to add.
 * @param[in] z Z coordinate of the point to add.
 */
void add_point(Points *points, index_t index, data_t x, data_t y, data_t z);

/**
 * @brief Finds the neighbours of a point in a certain radio of a point collection.
 *
 * @param[in] points Pointer to the collection of points.
 * @param[in] point_index Index of the point in the collection for which neighbors are being searched.
 * @param[out] result Pointer to RadiusResult where solution will be saved.
 * 
 * @note The point collection `points` must be initialized before calling this function.
 * @note Search by bruteforce, very inefficient.
 */
void find_radius_neighbors(const Points *points, index_t point_index, RadiusResult *result);

/**
 * @brief Frees the memory allocated for a set of points.
 *
 * @param[in,out] points Pointer to the Points structure to destroy.
 */
void destroy_points(Points *points);