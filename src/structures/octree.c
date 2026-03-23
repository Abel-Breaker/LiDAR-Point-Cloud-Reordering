#include "octree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>


/* ── Helpers privados ─────────────────────────────────────────────────────── */

/* Calcula el bounding box que engloba todos los puntos */
static AABB compute_global_aabb(const Points *pts)
{
	AABB bb = {
		.min = { DBL_MAX,  DBL_MAX,  DBL_MAX},
		.max = {-DBL_MAX, -DBL_MAX, -DBL_MAX}
	};

	for (size_t i = 0; i < pts->num_points; i++) {
		if (pts->x[i] < bb.min[0]) bb.min[0] = pts->x[i];
		if (pts->y[i] < bb.min[1]) bb.min[1] = pts->y[i];
		if (pts->z[i] < bb.min[2]) bb.min[2] = pts->z[i];
		if (pts->x[i] > bb.max[0]) bb.max[0] = pts->x[i];
		if (pts->y[i] > bb.max[1]) bb.max[1] = pts->y[i];
		if (pts->z[i] > bb.max[2]) bb.max[2] = pts->z[i];
	}

	return bb;
}

/* Determina en qué octante cae un punto respecto al centro del AABB.
 *
 * Octante codificado en bits:
 *   bit 0 → X (0 = izquierda, 1 = derecha)
 *   bit 1 → Y (0 = abajo,     1 = arriba)
 *   bit 2 → Z (0 = atrás,     1 = delante)
 */
static int get_octant(const Points *pts, size_t idx, const double center[3])
{
	int octant = 0;
	if (pts->x[idx] >= center[0]) octant |= 1;
	if (pts->y[idx] >= center[1]) octant |= 2;
	if (pts->z[idx] >= center[2]) octant |= 4;
	return octant;
}

/* Calcula el AABB del hijo dado el AABB del padre y el octante */
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

/* Crea un nodo hoja con los índices proporcionados */
static Octant *create_leaf(const AABB *bounds, const size_t *indices, size_t n)
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

/* Crea un nodo interno (sin puntos propios) */
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

/* Construcción recursiva del octree */
static Octant *octree_build(const Points *pts,
                                const AABB *bounds,
                                const size_t *indices,
                                size_t num_points,
                                int depth)
{
	if (num_points == 0) return NULL;

	/* Condición de hoja: pocos puntos o profundidad máxima alcanzada */
	if (num_points <= OCTREE_BUCKET_SIZE || depth >= OCTREE_MAX_DEPTH) {
		return create_leaf(bounds, indices, num_points);
	}

	/* Nodo interno: subdividir en 8 octantes */
	Octant *octant = create_internal(bounds);
	if (!octant) return NULL;

	double center[3] = {
		(bounds->min[0] + bounds->max[0]) / 2,
		(bounds->min[1] + bounds->max[1]) / 2,
		(bounds->min[2] + bounds->max[2]) / 2
	};

	/* Contar puntos por octante para evitar reallocs */
	size_t counts[8] = {0};
	for (size_t i = 0; i < num_points; i++) {
		int oct = get_octant(pts, indices[i], center);
		counts[oct]++;
	}

	/* Reservar arrays temporales para distribuir índices */
	size_t *buckets[8] = {NULL};
	size_t offsets[8]  = {0};
	for (int c = 0; c < 8; c++) {
		if (counts[c] > 0) {
			buckets[c] = calloc(counts[c], sizeof(*buckets[c]));
			if (!buckets[c]) {
				/* Limpieza parcial en caso de fallo */
				for (int j = 0; j < c; j++) free(buckets[j]);
				free(octant);
				return NULL;
			}
		}
	}

	/* Distribuir índices en los buckets correspondientes */
	for (size_t i = 0; i < num_points; i++) {
		int oct = get_octant(pts, indices[i], center);
		buckets[oct][offsets[oct]++] = indices[i];
	}

	/* Construir recursivamente cada hijo */
	for (int c = 0; c < 8; c++) {
		if (counts[c] > 0) {
			AABB cb = child_bounds(bounds, c);
			octant->children[c] = octree_build(pts, &cb, buckets[c], counts[c], depth + 1);
		}
		free(buckets[c]);
	}

	return octant;
}


void create_octree(Octree *octree, const Points *pts)
{
	octree->pts = pts;

	AABB global_bb = compute_global_aabb(pts);

	/* Array con todos los índices [0, num_points) */
	size_t *all_indices = malloc(pts->num_points * sizeof(*all_indices));
	if (!all_indices) {
		octree->root = NULL;
		return;
	}
	for (size_t i = 0; i < pts->num_points; i++) {
		all_indices[i] = i;
	}

	octree->root = octree_build(pts, &global_bb, all_indices, pts->num_points, 0);

	free(all_indices);
}

/* Liberación recursiva de nodos */
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
	octree_free_octant(octree->root);
	octree->root = NULL;
	octree->pts  = NULL;
}

// === Estadísticas === //
// ==================== //

static void collect_stats(const Octant *octant, int depth,
                          size_t *internal_count, size_t *leaf_count,
                          size_t *total_points, int *max_depth,
                          size_t *min_leaf_pts, size_t *max_leaf_pts)
{
	if (!octant) return;

	/* Un nodo es hoja si tiene point_indices (o no tiene hijos) */
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
	if (!octree->root) {
		printf("Octree vacío\n");
		return;
	}

	size_t internal_count = 0;
	size_t leaf_count     = 0;
	size_t total_points   = 0;
	int    max_depth      = 0;
	size_t min_leaf_pts   = (size_t)-1;
	size_t max_leaf_pts   = 0;

	collect_stats(octree->root, 0,
	              &internal_count, &leaf_count,
	              &total_points, &max_depth,
	              &min_leaf_pts, &max_leaf_pts);

	printf("=== Estadísticas del Octree ===\n");
	printf("  Nodos internos : %zu\n", internal_count);
	printf("  Hojas          : %zu\n", leaf_count);
	printf("  Puntos totales : %zu\n", total_points);
	printf("  Profundidad max: %d\n",  max_depth);
	if (leaf_count > 0) {
		printf("  Puntos/hoja min: %zu\n", min_leaf_pts);
		printf("  Puntos/hoja max: %zu\n", max_leaf_pts);
		printf("  Puntos/hoja avg: %.1f\n",
		       (double)total_points / (double)leaf_count);
	}
}
