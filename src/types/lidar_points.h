#pragma once
#include <math.h>
#include <stddef.h>

static inline double euclidian_distance_3d(double x2, double y2, double z2,
                                           double x1, double y1, double z1)
{
	return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
}

typedef struct{
    double *x;
    double *y;
    double *z;
    //uint16_t *intensity;
    //uint8_t *classification;
    size_t num_points;
} Points;


bool reserve_memory_points(Points *points, size_t number_of_points);

void add_point(Points *points, size_t index, double x, double y, double z);

void destroy_points(Points *points);
