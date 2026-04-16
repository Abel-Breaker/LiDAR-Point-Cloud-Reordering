#pragma once
#include "../../points_structures/octree.h"
#include "../../utils/parse_args.h"
#include <stddef.h>

/* Resultados de búsqueda por radio: array dinámico de índices y distancias. */
typedef struct {
	size_t *indices;
	bool *is_in_same_leaf;
	double *distances;
	size_t  count;
	size_t  capacity;
} RadiusResultPOC;

/* Rellena 'result' con todos los puntos a distancia <= radius del punto dado.
 * Inicializa el struct internamente; llamar radius_result_destroy al terminar. */
void octree_radius_search_POC(const Octree *octree, size_t point_index, double radius,
                          RadiusResultPOC *result);

void radius_result_destroy_POC(RadiusResultPOC *result);
