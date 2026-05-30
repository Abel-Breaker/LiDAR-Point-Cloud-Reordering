#pragma once
#include "../../../utils/types.h"

#ifdef __AVX512F__
index_t tfg_radius_search_avx2(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
				 index_t window, index_t search_start_index, data_t x, data_t y, data_t z,
				 data_t radius, index_t *restrict indices, data_t *restrict distances);
#endif