#pragma once
#include "../../../utils/types.h"

#ifdef __AVX512F__
/**
 * @brief Radius neighbour search using AVX-512 vectorization.
 *
 * @param[in] xs Pointer to the x coordinates array of points where to start search.
 * @param[in] ys Pointer to the y coordinates array of points where to start search..
 * @param[in] zs Pointer to the z coordinates array of points where to start search.
 * @param[in] window Number of points to examine.
 * @param[in] search_start_index Global index corresponding to the first point referenced by `xs`, `ys` and `zs`.
 * @param[in] x X coordinate of the query point.
 * @param[in] y Y coordinate of the query point.
 * @param[in] z Z coordinate of the query point.
 * @param[in] radius Search radius.
 * @param[out] indices Array where the indices of the matching points are stored.
 * @param[out] distances Array where the distances of the matching points are stored.
 *
 * @return Number of points founded.
 */
index_t tfg_radius_search_avx512(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
				 index_t window, index_t search_start_index, data_t x, data_t y, data_t z,
				 data_t radius, index_t *restrict indices, data_t *restrict distances);
#endif