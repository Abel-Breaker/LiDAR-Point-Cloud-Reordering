#ifndef PARSE_LIDAR_POINTS_H
#define PARSE_LIDAR_POINTS_H

#include "../types/lidar_points.h"

#ifdef __cplusplus
extern "C" {
#endif

int read_las_points(const char *filename, Points *pts);

#ifdef __cplusplus
}
#endif

#endif