#include "lidar_points.h"
#include <stdlib.h>

void destroy_points(Points *points){
    free(points->x);
    free(points->y);
    free(points->z);

    points->x = points->y = points->z = NULL;
    points->num_points = 0;
}