#ifndef PARSE_LIDAR_POINTS_H
#define PARSE_LIDAR_POINTS_H

#include "../types/lidar_points.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reads points from a LAS/LAZ file into a Points structure.
 *
 * @param[in] filename Path to the LAS/LAZ file.
 * @param[out] pts Pointer to the Points structure where the read points will be stored.
 * @return true if the file was read successfully, false otherwise.
 */
bool read_las_points(const char *filename, Points *pts);

#ifdef __cplusplus
}
#endif

#endif