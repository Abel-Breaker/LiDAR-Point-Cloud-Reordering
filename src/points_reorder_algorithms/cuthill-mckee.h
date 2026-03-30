#pragma once
#include "../structures/kd_tree.h"
#include "../types/lidar_points.h"

void reorder_points_cuthill_mckee(const KDTree *tree, Points *points);