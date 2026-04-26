#include "bruteforce.h"
#include <stdio.h>
#include <stdlib.h>

void find_radius_neighbors(const Points *points, size_t point_index, RadiusResult *result){

    const double x = points->x[point_index];
    const double y = points->y[point_index];
    const double z = points->z[point_index];

    result->count = 0;

    for(size_t i=0; i<points->num_points; ++i){
        double distance = euclidian_distance_3d(points->x[i], points->y[i], points->z[i], x, y, z);
        if(distance <= get_args()->radius_search){
            result->count++;
        }
    }

    if(result->count == 0){
        return;
    }

    reserves_memory_radius_result(result, result->count);

    size_t index = 0;
    for(size_t i=0; i<points->num_points; ++i){
        double distance = euclidian_distance_3d(points->x[i], points->y[i], points->z[i], x, y, z);
        if(distance <= get_args()->radius_search){
            result->indices[index] = i;
            result->distances[index] = distance;
            index++;
        }
    }
}

