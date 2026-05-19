#pragma once
#include "../../points_structures/octree.h"
#include "../../utils/parse_args.h"
#include <stddef.h>

/* Resultados de búsqueda por radio: array dinámico de índices y distancias. */
typedef struct {
	size_t *indices;
	double *distances;
	size_t  count;
	size_t  capacity;
} RadiusResultOctree;

/* Rellena 'result' con todos los puntos a distancia <= radius del punto dado.
 * Si  RadiusResultOctree es la primera vez que se usa tiene que estar correctamente inicializado a 0 (={})
 * Las siguientes llamadas puede reutilizar RadiusResultOctree sin problema para evitar malloc/free*/
void octree_radius_search(const Octree *octree, size_t point_index, double radius,
                          RadiusResultOctree *result);

size_t octree_radius_neighbor_count(const Octree *octree, size_t point_index, double radius);

void radius_result_destroy(RadiusResultOctree *result);
