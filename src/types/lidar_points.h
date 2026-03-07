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

void destroy_points(Points *points);
