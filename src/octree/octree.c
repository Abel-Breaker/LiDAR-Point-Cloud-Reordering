#include "octree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

/** 
 * Calculate the bounding box that encompasses all the points
*/
static AABB compute_global_aabb(const Points *points)
{
	AABB bb = {
		.min = { DBL_MAX,  DBL_MAX,  DBL_MAX},
		.max = {-DBL_MAX, -DBL_MAX, -DBL_MAX}
	};

	for (index_t i = 0; i < points->num_points; i++) {
		if (points->x[i] < bb.min[0]) bb.min[0] = points->x[i];
		if (points->y[i] < bb.min[1]) bb.min[1] = points->y[i];
		if (points->z[i] < bb.min[2]) bb.min[2] = points->z[i];
		if (points->x[i] > bb.max[0]) bb.max[0] = points->x[i];
		if (points->y[i] > bb.max[1]) bb.max[1] = points->y[i];
		if (points->z[i] > bb.max[2]) bb.max[2] = points->z[i];
	}

	return bb;
}

/**
 * Determines the octant in which a point falls relative to the center of the AABB.
 * 
 * Octant encoded in bits:
 * 	bit 0 → X (0 = left, 1 = right)
 * 	bit 1 → Y (0 = down, 1 = up)
 * 	bit 2 → Z (0 = back, 1 = front)
*/
static int get_octant(const Points *points, index_t idx, const double center[3])
{
	int octant = 0;
	if (points->x[idx] >= center[0]) octant |= 1;
	if (points->y[idx] >= center[1]) octant |= 2;
	if (points->z[idx] >= center[2]) octant |= 4;
	return octant;
}

/**
 * Calculate the AABB of the son given the AABB of the father and the octant
*/
static AABB child_bounds(const AABB *parent, int octant)
{
	double cx = (parent->min[0] + parent->max[0]) / 2;
	double cy = (parent->min[1] + parent->max[1]) / 2;
	double cz = (parent->min[2] + parent->max[2]) / 2;

	AABB child;
	child.min[0] = (octant & 1) ? cx : parent->min[0];
	child.max[0] = (octant & 1) ? parent->max[0] : cx;
	child.min[1] = (octant & 2) ? cy : parent->min[1];
	child.max[1] = (octant & 2) ? parent->max[1] : cy;
	child.min[2] = (octant & 4) ? cz : parent->min[2];
	child.max[2] = (octant & 4) ? parent->max[2] : cz;

	return child;
}

/**
 * Create a leaf node with the provided indices
*/
static Octant *create_leaf(const AABB *bounds, const index_t *indices, index_t n)
{
	Octant *octant = malloc(sizeof(*octant));
	if (!octant) return NULL;

	memset(octant->children, 0, sizeof(octant->children));
	octant->bounds = *bounds;
	octant->num_points = n;

	octant->point_indices = malloc(n * sizeof(*octant->point_indices));
	if (!octant->point_indices) {
		free(octant);
		return NULL;
	}
	memcpy(octant->point_indices, indices, n * sizeof(*indices));

	return octant;
}

/**
 * Create an internal node (without its own points)
*/
static Octant *create_internal(const AABB *bounds)
{
	Octant *octant = malloc(sizeof(*octant));
	if (!octant) return NULL;

	memset(octant->children, 0, sizeof(octant->children));
	octant->bounds = *bounds;
	octant->point_indices = NULL;
	octant->num_points = 0;

	return octant;
}

/**
 * Recursive construction of the octree
*/
static Octant *octree_build(const Points *points,
                                const AABB *bounds,
                                const index_t *indices,
                                index_t num_points,
                                int depth)
{
	if (num_points == 0) return NULL;

	// Leaf condition: few points or maximum depth reached
	if (num_points <= OCTREE_BUCKET_SIZE || depth >= OCTREE_MAX_DEPTH) {
		return create_leaf(bounds, indices, num_points);
	}

	// Internal node: subdivide into 8 octants
	Octant *octant = create_internal(bounds);
	if (!octant) return NULL;

	double center[3] = {
		(bounds->min[0] + bounds->max[0]) / 2,
		(bounds->min[1] + bounds->max[1]) / 2,
		(bounds->min[2] + bounds->max[2]) / 2
	};

	// Count points per octant to avoid reallocs
	index_t counts[8] = {0};
	for (index_t i = 0; i < num_points; i++) {
		int oct = get_octant(points, indices[i], center);
		counts[oct]++;
	}

	// Reserve temporary arrays to distribute indices
	index_t *buckets[8] = {NULL};
	index_t offsets[8]  = {0};
	for (int c = 0; c < 8; c++) {
		if (counts[c] > 0) {
			buckets[c] = calloc(counts[c], sizeof(*buckets[c]));
			if (!buckets[c]) {
				// Partial cleaning in case of failure
				for (int j = 0; j < c; j++) free(buckets[j]);
				free(octant);
				return NULL;
			}
		}
	}

	// Distribute indices to the corresponding buckets
	for (index_t i = 0; i < num_points; i++) {
		int oct = get_octant(points, indices[i], center);
		buckets[oct][offsets[oct]++] = indices[i];
	}

	// Recursively build each child
	for (int c = 0; c < 8; c++) {
		if (counts[c] > 0) {
			AABB cb = child_bounds(bounds, c);
			octant->children[c] = octree_build(points, &cb, buckets[c], counts[c], depth + 1);
		}
		free(buckets[c]);
	}

	return octant;
}


void create_octree(Octree *octree, const Points *points)
{
	octree->points = points;

	AABB global_bb = compute_global_aabb(points);

	// Array with all indices [0, num_points)
	index_t *all_indices = malloc(points->num_points * sizeof(*all_indices));
	if (!all_indices) {
		octree->root = NULL;
		return;
	}
	for (index_t i = 0; i < points->num_points; i++) {
		all_indices[i] = i;
	}

	octree->root = octree_build(points, &global_bb, all_indices, points->num_points, 0);

	free(all_indices);
}

/**
 * Recursive node release
 */
static void octree_free_octant(Octant *octant)
{
	if (!octant) return;

	for (int c = 0; c < 8; c++) {
		octree_free_octant(octant->children[c]);
	}
	free(octant->point_indices);
	free(octant);
}

void destroy_octree(Octree *octree)
{
	if(!octree) return;
	octree_free_octant(octree->root);
	octree->root = NULL;
	octree->points  = NULL;
}


/**
 * Get stadistics
 */
static void collect_stats(const Octant *octant, int depth,
                          index_t *internal_count, index_t *leaf_count,
                          index_t *total_points, int *max_depth,
                          index_t *min_leaf_pts, index_t *max_leaf_pts)
{
	if (!octant) return;

	// A node is a leaf node if it has point_indices (or no children)
	bool is_leaf = (octant->point_indices != NULL);

	if (is_leaf) {
		(*leaf_count)++;
		*total_points += octant->num_points;
		if (depth > *max_depth) *max_depth = depth;
		if (octant->num_points < *min_leaf_pts) *min_leaf_pts = octant->num_points;
		if (octant->num_points > *max_leaf_pts) *max_leaf_pts = octant->num_points;
	} else {
		(*internal_count)++;
		for (int c = 0; c < 8; c++) {
			collect_stats(octant->children[c], depth + 1,
			              internal_count, leaf_count,
			              total_points, max_depth,
			              min_leaf_pts, max_leaf_pts);
		}
	}
}

void octree_print_stats(const Octree *octree)
{
	if(!octree) return;
	
	if (!octree->root) {
		printf("Octree vacío\n");
		return;
	}

	index_t internal_count = 0;
	index_t leaf_count     = 0;
	index_t total_points   = 0;
	int    max_depth      = 0;
	index_t min_leaf_pts   = (index_t)-1;
	index_t max_leaf_pts   = 0;

	collect_stats(octree->root, 0,
	              &internal_count, &leaf_count,
	              &total_points, &max_depth,
	              &min_leaf_pts, &max_leaf_pts);

	printf("=== Estadísticas del Octree ===\n");
	printf("  Nodos internos : %zu\n", (size_t) internal_count);
	printf("  Hojas          : %zu\n", (size_t) leaf_count);
	printf("  Puntos totales : %zu\n", (size_t) total_points);
	printf("  Profundidad max: %d\n",  max_depth);
	if (leaf_count > 0) {
		printf("  Puntos/hoja min: %zu\n", (size_t) min_leaf_pts);
		printf("  Puntos/hoja max: %zu\n", (size_t) max_leaf_pts);
		printf("  Puntos/hoja avg: %.1f\n",
		       (double)total_points / (double)leaf_count);
	}
}
