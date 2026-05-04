#pragma once
#include "../../types/neighborhood_matrix_mix/neighborhood_matrix.h"
#include "../../utils/auxiliar_structures/radius_result.h"

void tfg_radius_search_opt(const Points *points, size_t index, size_t bandwith_left, size_t bandwith_right, RadiusResult *result);

void tfg_print_timing_stats_opt(void);