#pragma once
#include <math.h>
#include <stddef.h>

#define DIMENSIONS 3

enum axis { X = 0, Y = 1, Z = 2 };

/**
 * Computes the Euclidean distance between two points in 3D space.
 */
static inline double euclidian_distance_3d(double x2, double y2, double z2,
                                           double x1, double y1, double z1)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
    //return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1); // TODO: Change plane_distance kd-tree
}

typedef struct{
    double *x;
    double *y;
    double *z;
    size_t num_points;
} Points;

/**
 * Allocates memory for a set of points.
 *
 * @param[out] points Pointer to the Points structure to allocate.
 * @param[in] number_of_points Number of points to reserve space for.
 * @return true if memory allocation was successful, false otherwise.
 */
bool reserve_memory_points(Points *points, size_t number_of_points);

/**
 * Adds a point to a set of points at a specified index.
 *
 * @param[in,out] points Pointer to the Points structure.
 * @param[in] index Index at which the point will be added.
 * @param[in] x, y, z Coordinates of the point to add.
 */
void add_point(Points *points, size_t index, double x, double y, double z);

/**
 * Frees the memory allocated for a set of points.
 *
 * @param[in,out] points Pointer to the Points structure to destroy.
 */
void destroy_points(Points *points);