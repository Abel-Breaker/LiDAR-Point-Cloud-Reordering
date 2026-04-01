#pragma once
#include "../types/lidar_points.h"
#include <stddef.h>

// Maximum number of points per sheet before subdividing
#define OCTREE_BUCKET_SIZE 128

// Maximum tree depth (avoids infinite recursion with coincident points)
#define OCTREE_MAX_DEPTH 32


// Bounding box aligned with the axes
typedef struct {
	// x y z
	double min[DIMENSIONS]; 
	double max[DIMENSIONS];
} AABB;

typedef struct Octant {
	struct Octant *children[8];     // NULL in leafs        
	size_t *point_indices;          // Indices (only leafs) 
	size_t num_points;              // Points on this leaf 
	AABB bounds;
} Octant;

typedef struct Octree {
	Octant *root;
	const Points *pts;
} Octree;

/**
 * Creates an octree from a given collection of points.
 *
 * @param[out] octree Pointer to the Octree structure to initialize.
 * @param[in] pts Pointer to the Points structure used to build the octree.
 *
 * @note The point collection `pts` must be initialized before calling this function.
 */
void create_octree(Octree *octree, const Points *pts);

/**
 * Destroys an octree, releasing all associated resources.
 *
 * @param[in,out] octree Pointer to the Octree structure to destroy.
 */
void destroy_octree(Octree *octree);

/**
 * Prints statistical information about the octree.
 *
 * @param[in] octree Pointer to the Octree structure.
 */
void octree_print_stats(const Octree *octree);
