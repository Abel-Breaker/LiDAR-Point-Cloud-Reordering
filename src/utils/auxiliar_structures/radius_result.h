#pragma once
#include <stddef.h>

typedef struct {
	size_t *indices;
	double *distances;
	size_t  count;
} RadiusResult;

void reserves_memory_radius_result(RadiusResult *result, size_t num_elements);

void destroy_radius_result(RadiusResult *result);