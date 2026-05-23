#include "radius_result.h"
#include <stdlib.h>

void reserves_memory_radius_result(RadiusResult *result, size_t num_elements){
    result->count = num_elements;
    result->indices = malloc(sizeof(*(result->indices)) * result->count);
    result->distances = malloc(sizeof(*(result->distances)) * result->count);
}

void destroy_radius_result(RadiusResult *result){
    free(result->distances);
    free(result->indices);
    result->count = 0;
    result->distances = nullptr;
    result->indices = nullptr;
}