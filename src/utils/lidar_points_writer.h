#ifndef LIDAR_POINTS_WRITER_H
#define LIDAR_POINTS_WRITER_H

#include "../types/lidar_points.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Writes points to a LAS file.
 *
 * @param[in] filename Path to the LAS file to create.
 * @param[in] pts Pointer to the Points structure where the points are stored.
 */
void write_las_points(const char *filename, const Points *pts);

#ifdef __cplusplus
}
#endif

#endif