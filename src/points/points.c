#include "points.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

bool reserve_memory_points(Points *points, index_t number_of_points)
{
    // Convert number_of_points to a multple of 64
    index_t size = number_of_points * sizeof(double);
   index_t aligned_size = (size + (index_t)63) & ~(index_t)63; //

    points->x = aligned_alloc(64, aligned_size);
    points->y = aligned_alloc(64, aligned_size);
    points->z = aligned_alloc(64, aligned_size);

    if (!points->x || !points->y || !points->z) {
        return false;
    }

    points->num_points = number_of_points;
    return true;
}

void add_point(Points *points, index_t index, double x, double y, double z)
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