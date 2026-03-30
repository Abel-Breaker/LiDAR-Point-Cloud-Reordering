#include "lidar_points.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

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

void swap_points(Points *points, const unsigned int *permutations){
    double *new_x = malloc(points->num_points * sizeof(double));
    double *new_y = malloc(points->num_points * sizeof(double));
    double *new_z = malloc(points->num_points * sizeof(double));

    for (size_t i = 0; i < points->num_points; ++i){
        new_x[i] = points->x[permutations[i]];
        new_y[i] = points->y[permutations[i]];
        new_z[i] = points->z[permutations[i]];
    }

    free(points->x);
    free(points->y);
    free(points->z);

    points->x = new_x;
    points->y = new_y;
    points->z = new_z;
}

void destroy_points(Points *points)
{
    if(!points) return;
    
	if (points->x) free(points->x);
    if (points->y) free(points->y);
    if (points->z) free(points->z);
	
	points->x = points->y = points->z = NULL;
	points->num_points = 0;
}