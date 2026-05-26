#include "points.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../utils/parse_args.h"

void find_radius_neighbors(const Points *points, index_t point_index, RadiusResult *result){

    const data_t x = points->x[point_index];
    const data_t y = points->y[point_index];
    const data_t z = points->z[point_index];

    result->count = 0;

    for(index_t i=0; i<points->num_points; ++i){
        data_t distance = euclidian_distance_3d(points->x[i], points->y[i], points->z[i], x, y, z);
        if(distance <= get_args()->radius_search){
            result->count++;
        }
    }

    if(result->count == 0){
        return;
    }

    reserves_memory_radius_result(result, result->count);

    index_t index = 0;
    for(index_t i=0; i<points->num_points; ++i){
        data_t distance = euclidian_distance_3d(points->x[i], points->y[i], points->z[i], x, y, z);
        if(distance <= get_args()->radius_search){
            result->indices[index] = i;
            result->distances[index] = distance;
            index++;
        }
    }
}

