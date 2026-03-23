#pragma once
#include "../types/lidar_points.h"
#include <stddef.h>

/* Número máximo de puntos por hoja antes de subdividir */
#define OCTREE_BUCKET_SIZE 128

/* Profundidad máxima del árbol (evita recursión infinita con puntos coincidentes) */
#define OCTREE_MAX_DEPTH 32


/* Bounding box alineada con los ejes */
typedef struct {
	// x y z
	double min[3]; 
	double max[3];
} AABB;

typedef struct Octant {
	struct Octant *children[8];     // NULL en hojas        
	size_t *point_indices;          // índices (solo hojas) 
	size_t num_points;              // puntos en esta hoja  
	AABB bounds;
} Octant;

typedef struct Octree {
	Octant *root;
	const Points *pts;
} Octree;

void create_octree(Octree *octree, const Points *pts);
void destroy_octree(Octree *octree);
void octree_print_stats(const Octree *octree);
