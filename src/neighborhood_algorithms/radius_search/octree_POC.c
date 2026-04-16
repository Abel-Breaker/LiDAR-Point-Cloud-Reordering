#include "octree_POC.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Comprueba si el punto está dentro del bounding box (inclusivo). */
static inline bool aabb_contains(const AABB *box,
                                  double px, double py, double pz)
{
	return px >= box->min[0] && px <= box->max[0]
	    && py >= box->min[1] && py <= box->max[1]
	    && pz >= box->min[2] && pz <= box->max[2];
}

/* Distancia mínima desde el punto (px,py,pz) al bounding box.
 * Si el punto está dentro, devuelve 0. */
static inline double aabb_min_dist(const AABB *box,
                                   double px, double py, double pz)
{
	double dx, dy, dz;
	dx = dy = dz = 0;

	if      (px < box->min[0]) dx = box->min[0] - px;
	else if (px > box->max[0]) dx = px - box->max[0];

	if      (py < box->min[1]) dy = box->min[1] - py;
	else if (py > box->max[1]) dy = py - box->max[1];

	if      (pz < box->min[2]) dz = box->min[2] - pz;
	else if (pz > box->max[2]) dz = pz - box->max[2];

	return sqrt(dx * dx + dy * dy + dz * dz);
}


/* Añade un punto al resultado, redoblando capacidad si es necesario. */
static bool radius_result_push(RadiusResultPOC *res, size_t idx, double dist)
{
	if (res->count == res->capacity) {
		size_t new_cap = res->capacity == 0 ? 64u : res->capacity * 2u;
		size_t *ni = realloc(res->indices,  new_cap * sizeof(*res->indices));
		double *nd = realloc(res->distances, new_cap * sizeof(*res->distances));
		if (!ni || !nd) {
			free(ni);
			free(nd);
			return false;
		}
		res->indices   = ni;
		res->distances = nd;
		res->capacity  = new_cap;
	}
	res->indices[res->count]   = idx;
	res->distances[res->count] = dist;
	res->count++;
	return true;
}

static void radius_traverse(const Octree *octree, const Octant *octant,
                            double px, double py, double pz,
                            double radius, RadiusResultPOC *result)
{
	if (!octant) return;

	/* Poda: si la esquina más cercana del AABB ya supera el radio, no hay
	 * ningún punto de este subárbol que pueda estar dentro. */
	if (aabb_min_dist(&octant->bounds, px, py, pz) > radius) return;

	/* Nodo hoja: evaluar todos los puntos del bucket. */
	if (octant->point_indices) {
		for (size_t i = 0; i < octant->num_points; ++i) {
			size_t idx = octant->point_indices[i];
			double dist = euclidian_distance_3d(
			    octree->pts->x[idx], octree->pts->y[idx], octree->pts->z[idx],
			    px, py, pz);
			if (dist <= radius)
				radius_result_push(result, idx, dist);
		}
		return;
	}

	/* Nodo interno: visitar primero el hijo que contiene la consulta. */
	int containing_child = -1;
	for (int c = 0; c < 8; ++c) {
		if (!octant->children[c]) continue;
		if (aabb_contains(&octant->children[c]->bounds, px, py, pz)) {
			containing_child = c;
			radius_traverse(octree, octant->children[c], px, py, pz, radius, result);
			break;
		}
	}
	for (int c = 0; c < 8; ++c) {
		if (!octant->children[c] || c == containing_child) continue;
		radius_traverse(octree, octant->children[c], px, py, pz, radius, result);
	}
}

void octree_radius_search_POC(const Octree *octree, size_t point_index, double radius,
                          RadiusResultPOC *result)
{
	result->indices   = nullptr;
	result->distances = nullptr;
	result->count     = 0;
	result->capacity  = 0;

	double px = octree->pts->x[point_index];
	double py = octree->pts->y[point_index];
	double pz = octree->pts->z[point_index];

	radius_traverse(octree, octree->root, px, py, pz, radius, result);
}

void radius_result_destroy_POC(RadiusResultPOC *result)
{
	free(result->indices);
	free(result->distances);
	result->indices   = nullptr;
	result->distances = nullptr;
	result->count     = 0;
	result->capacity  = 0;
}
