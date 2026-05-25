#pragma once
#include "../types.h"

typedef struct {
	index_t *indices;
	double *distances;
	index_t  count;
} RadiusResult;

void reserves_memory_radius_result(RadiusResult *result, index_t num_elements);

void destroy_radius_result(RadiusResult *result);