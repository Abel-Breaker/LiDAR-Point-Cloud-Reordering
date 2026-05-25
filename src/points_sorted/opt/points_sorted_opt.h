#pragma once
#include "../../points/points.h"
#include "../../octree/octree.h"
#include "../../../utils/auxiliar_structures/radius_result.h"
#include "../../../utils/types.h"
#include <math.h>
#include <stddef.h>

typedef struct{
    Points *points;
    index_t *bandwith_left;
	index_t *bandwith_right;
} Points_TFG_opt;
