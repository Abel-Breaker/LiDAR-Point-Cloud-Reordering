#include "radius_result.h"
#include <stdlib.h>

void reserves_memory_radius_result(RadiusResult *result, index_t num_elements)
{
	result->count = num_elements;
	result->indices = aligned_alloc(64, ((sizeof(*(result->indices)) * result->count + 63) / 64) * 64);
	result->distances = aligned_alloc(64, ((sizeof(*(result->distances)) * result->count + 63) / 64) * 64);
}

void destroy_radius_result(RadiusResult *result)
{
	free(result->distances);
	free(result->indices);
	result->count = 0;
	result->distances = nullptr;
	result->indices = nullptr;
}