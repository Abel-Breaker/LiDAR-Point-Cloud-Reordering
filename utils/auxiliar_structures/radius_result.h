#pragma once
#include "../types.h"

typedef struct {
	index_t *indices;
	data_t *distances;
	index_t  count;
} RadiusResult;

/**
 * @brief Allocates memory for a radius search result.
 *
 * @param result Pointer to the RadiusResult structure.
 * @param num_elements Number of elements to allocate space for.
 * 
 * @note RadiusResult can be reusable.
 */
void reserves_memory_radius_result(RadiusResult *result, index_t num_elements);

/**
 * @brief Releases all resources associated with a radius search result.
 *
 * @param result Pointer to the RadiusResult structure to destroy.
 */
void destroy_radius_result(RadiusResult *result);