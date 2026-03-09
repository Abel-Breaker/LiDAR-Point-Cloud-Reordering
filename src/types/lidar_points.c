#include "lidar_points.h"
#include <stdlib.h>
#include <stddef.h>

void reserve_memory_points(Points *points, size_t number_of_points){
    points->x = malloc(number_of_points * sizeof(*(points->x)));
	points->y = malloc(number_of_points * sizeof(*(points->y)));
	points->z = malloc(number_of_points * sizeof(*(points->z)));
	if (!points->x || !points->y || !points->z) {
		return;
	}
    points->num_points = number_of_points;
}

/*void add_point(Points *points){
    return;
}*/

void destroy_points(Points *points){
    free(points->x);
    free(points->y);
    free(points->z);

    points->x = points->y = points->z = NULL;
    points->num_points = 0;
}