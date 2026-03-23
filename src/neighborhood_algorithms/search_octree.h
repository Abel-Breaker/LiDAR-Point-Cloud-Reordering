#pragma once
#include "../structures/octree.h"
#include "../utils/parse_args.h"
#include <stddef.h>

void start_octree_knearest(const Octree *octree, size_t point_index,
                           size_t neighbours_index[K],
                           double neighbours_distances[K]);

/* Resultados de búsqueda por radio: array dinámico de índices y distancias. */
typedef struct {
	size_t *indices;
	double *distances;
	size_t  count;
	size_t  capacity;
} RadiusResult;

/* Rellena 'result' con todos los puntos a distancia <= radius del punto dado.
 * Inicializa el struct internamente; llamar radius_result_destroy al terminar. */
void octree_radius_search(const Octree *octree, size_t point_index, double radius,
                          RadiusResult *result);

void radius_result_destroy(RadiusResult *result);
