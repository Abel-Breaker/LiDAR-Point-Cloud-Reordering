#pragma once
#include "../types/lidar_points.h"
#include "../points_structures/kd_tree.h"

void reorder_random(const KDTree *tree, Points *points, Points *new_points);