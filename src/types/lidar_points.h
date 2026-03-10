#pragma once
#include <stddef.h>

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
