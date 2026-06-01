#pragma once
#include "../../utils/parse_args.h"
#include "../../utils/types.h"
#include "../points/points.h"

// Maximum number of points per sheet before subdividing
#define OCTREE_BUCKET_SIZE 128

// Maximum tree depth (avoids infinite recursion with coincident points)
#define OCTREE_MAX_DEPTH 32

// Bounding box aligned with the axes
typedef struct {
	data_t min[DIMENSIONS];
	data_t max[DIMENSIONS];
} AABB;

typedef struct Octant {
	struct Octant *children[8]; // NULL in leafs
	index_t *point_indices;	    // Indices (only leafs)
	index_t num_points;	    // Points on this leaf
	AABB bounds;
} Octant;

typedef struct Octree {
	Octant *root;
	const Points *points;
} Octree;

/**
 * @brief Creates an octree from a given collection of points.
 *
 * @param[out] octree Pointer to the Octree structure to initialize.
 * @param[in] points Pointer to the Points structure used to build the octree.
 *
 * @note The point collection `points` must be initialized before calling this function.
 */
void create_octree(Octree *octree, const Points *points);

/**
 * @brief Destroys an octree, releasing all associated resources.
 *
 * @param[in,out] octree Pointer to the Octree structure to destroy.
 */
void destroy_octree(Octree *octree);

/**
 * @brief Prints statistical information about the octree.
 *
 * @param[in] octree Pointer to the Octree structure.
 */
void octree_print_stats(const Octree *octree);

typedef struct {
	index_t *indices;
	data_t *distances;
	index_t count;
	index_t capacity;
} RadiusResultOctree;

 /**
 * @brief Search neighbours of a point in a radius.
 * 
 * @param[in] octree Pointer to the Octree structure.
 * @param[in] point_index Index of the point on which to search for neighbors.
 * @param[out] result Pointer to RadiusResultOctree where solution will be saved.
 * 
 * @note Argument `result` has to be initialized. It can be reused.
 * @note Argument `result` has to be destroyed when it is no longer going to be used.
 */
void octree_radius_search(const Octree *octree, index_t point_index, data_t radius, RadiusResultOctree *result);

/**
 * @brief Counts the number of neighbours that a point has in a radius.
 * 
 * @param[in] octree Pointer to the Octree structure.
 * @param[in] point_index Index of the point on which to search for neighbors.
 * @param[in] radius Radius to search points.
 * 
 * @return Number of neighbours for the point in the radius.
 */
index_t octree_radius_neighbor_count(const Octree *octree, index_t point_index, data_t radius);

/**
 * @brief Free memory for RadiusResultOctree.
 * 
 * @param[in] result Pointer to a RadiusResultOctree to free.
 */
void radius_result_destroy(RadiusResultOctree *result);
