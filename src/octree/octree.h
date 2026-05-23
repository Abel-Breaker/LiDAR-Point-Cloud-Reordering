#pragma once
#include "../../utils/parse_args.h"
#include "../points/points.h"
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
	struct Octant *children[8]; // NULL in leafs
	size_t *point_indices;	    // Indices (only leafs)
	size_t num_points;	    // Points on this leaf
	AABB bounds;
} Octant;

typedef struct Octree {
	Octant *root;
	const Points *points;
} Octree;

/**
 * Creates an octree from a given collection of points.
 *
 * @param[out] octree Pointer to the Octree structure to initialize.
 * @param[in] points Pointer to the Points structure used to build the octree.
 *
 * @note The point collection `points` must be initialized before calling this function.
 */
void create_octree(Octree *octree, const Points *points);

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

/* Resultados de búsqueda por radio: array dinámico de índices y distancias. */
typedef struct {
	size_t *indices;
	double *distances;
	size_t count;
	size_t capacity;
} RadiusResultOctree;

/* Rellena 'result' con todos los puntos a distancia <= radius del punto dado.
 * Si  RadiusResultOctree es la primera vez que se usa tiene que estar correctamente inicializado a 0 (={})
 * Las siguientes llamadas puede reutilizar RadiusResultOctree sin problema para evitar malloc/free*/
void octree_radius_search(const Octree *octree, size_t point_index, double radius, RadiusResultOctree *result);

size_t octree_radius_neighbor_count(const Octree *octree, size_t point_index, double radius);

void radius_result_destroy(RadiusResultOctree *result);
