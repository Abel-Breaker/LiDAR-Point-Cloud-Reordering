#pragma once
#include "../points_structures/kd_tree.h"
#include "../types/lidar_points.h"

void reorder_bfs_sort_by_distance(const KDTree *tree, const Points *points, Points *new_points);

void reorder_bfs_sort_by_distance_reverse(const KDTree *tree, const Points *points, Points *new_points);

void reorder_bfs_sort_by_index(const KDTree *tree, const Points *points, Points *new_points);