#pragma once
#include "../types/lidar_points.h"
#include "knn_kd_tree.h"

void find_point_neighbors(Points *points, size_t point_index, size_t neighbours_index[K]);