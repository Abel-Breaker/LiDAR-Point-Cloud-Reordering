#include "lidar_points.h"
#include <stddef.h>
#include <stdlib.h>

bool reserve_memory_points(Points *points, size_t number_of_points)
{
	points->x = malloc(number_of_points * sizeof(*(points->x)));
	points->y = malloc(number_of_points * sizeof(*(points->y)));
	points->z = malloc(number_of_points * sizeof(*(points->z)));
	if (!points->x || !points->y || !points->z) {
		return false;
	}
	points->num_points = number_of_points;

	return true;
}

void add_point(Points *points, size_t index, double x, double y, double z)
{
	points->x[index] = x;
	points->y[index] = y;
	points->z[index] = z;
}

void destroy_points(Points *points)
{
    if(!points) return;
    
	free(points->x);
	free(points->y);
	free(points->z);

	points->x = points->y = points->z = NULL;
	points->num_points = 0;
}