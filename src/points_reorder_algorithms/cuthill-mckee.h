#pragma once
#include "../points_structures/octree.h"
#include "../types/lidar_points.h"

void reorder_cuthill_mckee(const Octree *tree, const Points *points, Points *new_points);