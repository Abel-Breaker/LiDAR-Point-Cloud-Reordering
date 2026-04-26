#pragma once
#include "../types/lidar_points.h"
#include "../types/neighborhood_matrix_mix/neighborhood_matrix.h"

void reorder_cuthill_mckee(const struct matrix_t *matrix, Points *new_points);