#include "avx512.h"
#include <immintrin.h>
#include <stdio.h>
/**
 * Premature pruning: This function is memory bound because of the load, so we do
 * 					  premature pruning to avoid loads.
 * Unrroling x2 seems to work better
 */
#if defined(__AVX512F__) && defined(USE_FLOAT)
index_t tfg_radius_search_avx512(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
				 index_t window, index_t search_start_index, data_t x, data_t y, data_t z,
				 data_t radius, index_t *restrict indices, data_t *restrict distances)
{
	index_t elements_count = 0;

	// Create a vector with the position of the point to compare (x0, y0, z0)
	const __m512 x0_vector = _mm512_set1_ps(x);
	const __m512 y0_vector = _mm512_set1_ps(y);
	const __m512 z0_vector = _mm512_set1_ps(z);

	// Create a vector with the radius
	const __m512 radius_vector = _mm512_set1_ps(radius);

	const __m512i increment = _mm512_set1_epi32(16);
	const __m512i increment_32 = _mm512_set1_epi32(32);

	// Create base index (where search starts)
	const __m512i idx_offsets = _mm512_set_epi32(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
	__m512i base_idx = _mm512_add_epi32(_mm512_set1_epi32(search_start_index), idx_offsets);

	for (index_t i = 0; i + 31 < window; i += 32) {
		// X²
		__m512 x_vector_result = _mm512_sub_ps(x0_vector, _mm512_load_ps(xs + i));
		__m512 x2_vector_result = _mm512_sub_ps(x0_vector, _mm512_load_ps(xs + i + 16));
		__m512 result_vec = _mm512_mul_ps(x_vector_result, x_vector_result);
		__m512 result_vec2 = _mm512_mul_ps(x2_vector_result, x2_vector_result);

		if (_mm512_cmp_ps_mask(result_vec, radius_vector, _CMP_LE_OQ) |
		    _mm512_cmp_ps_mask(result_vec2, radius_vector, _CMP_LE_OQ)) {
				// X² + Y²
			__m512 y_vector_result = _mm512_sub_ps(y0_vector, _mm512_load_ps(ys + i));
			__m512 y2_vector_result = _mm512_sub_ps(y0_vector, _mm512_load_ps(ys + i + 16));
			result_vec = _mm512_fmadd_ps(y_vector_result, y_vector_result, result_vec);
			result_vec2 = _mm512_fmadd_ps(y2_vector_result, y2_vector_result, result_vec2);

			if (_mm512_cmp_ps_mask(result_vec, radius_vector, _CMP_LE_OQ) |
			    _mm512_cmp_ps_mask(result_vec2, radius_vector, _CMP_LE_OQ)) {
					// X² + Y² + Z²
				__m512 z_vector_result = _mm512_sub_ps(z0_vector, _mm512_load_ps(zs + i));
				__m512 z2_vector_result = _mm512_sub_ps(z0_vector, _mm512_load_ps(zs + i + 16));
				result_vec = _mm512_fmadd_ps(z_vector_result, z_vector_result, result_vec);
				result_vec2 = _mm512_fmadd_ps(z2_vector_result, z2_vector_result, result_vec2);

				// Compare distance <= radius
				__mmask16 mask = _mm512_cmp_ps_mask(result_vec, radius_vector, _CMP_LE_OQ);
				__mmask16 mask2 = _mm512_cmp_ps_mask(result_vec2, radius_vector, _CMP_LE_OQ);

				if (mask) {
					// Save matching results
					_mm512_mask_compressstoreu_epi32(indices + elements_count, mask, base_idx);
					_mm512_mask_compressstoreu_ps(distances + elements_count, mask, result_vec);

					// Count valid elements
					elements_count += __builtin_popcount(mask);
				}
				if (mask2) {
					// Save matching results
					_mm512_mask_compressstoreu_epi32(indices + elements_count, mask2,
									 _mm512_add_epi32(base_idx, increment));
					_mm512_mask_compressstoreu_ps(distances + elements_count, mask2, result_vec2);

					// Count valid elements
					elements_count += __builtin_popcount(mask2);
				}
			}
		}
		// Increment indices
		base_idx = _mm512_add_epi32(base_idx, increment_32);
	}

	return elements_count;
}
#elif defined(__AVX512F__)
index_t tfg_radius_search_avx512(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
				 index_t window, index_t search_start_index, data_t x, data_t y, data_t z,
				 data_t radius, index_t *restrict indices, data_t *restrict distances)
{
	index_t elements_count = 0;

	// Create a vector with the position of the point to compare (x0, y0, z0)
	const __m512d x0_vector = _mm512_set1_pd(x);
	const __m512d y0_vector = _mm512_set1_pd(y);
	const __m512d z0_vector = _mm512_set1_pd(z);

	const __m512d radius_vector = _mm512_set1_pd(radius);

	const __m512i increment = _mm512_set1_epi64(8);
	const __m512i increment_16 = _mm512_set1_epi64(16);

	// Create base index (where search starts)
	const __m512i idx_offsets = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
	__m512i base_idx = _mm512_add_epi64(_mm512_set1_epi64(search_start_index), idx_offsets);

	for (index_t i = 0; i + 15 < window; i += 16) {
		// X²
		__m512d xr = _mm512_sub_pd(x0_vector, _mm512_load_pd(xs + i));
		__m512d xr2 = _mm512_sub_pd(x0_vector, _mm512_load_pd(xs + i + 8));
		__m512d result_vec = _mm512_mul_pd(xr, xr);
		__m512d result_vec2 = _mm512_mul_pd(xr2, xr2);

		if (_mm512_cmp_pd_mask(result_vec, radius_vector, _CMP_LE_OQ) |
		    _mm512_cmp_pd_mask(result_vec2, radius_vector, _CMP_LE_OQ)) {
			// X² + Y²
			__m512d yr = _mm512_sub_pd(y0_vector, _mm512_load_pd(ys + i));
			__m512d yr2 = _mm512_sub_pd(y0_vector, _mm512_load_pd(ys + i + 8));
			result_vec = _mm512_fmadd_pd(yr, yr, result_vec);
			result_vec2 = _mm512_fmadd_pd(yr2, yr2, result_vec2);

			if (_mm512_cmp_pd_mask(result_vec, radius_vector, _CMP_LE_OQ) |
			    _mm512_cmp_pd_mask(result_vec2, radius_vector, _CMP_LE_OQ)) {
				// X² + Y² + Z²
				__m512d zr = _mm512_sub_pd(z0_vector, _mm512_load_pd(zs + i));
				__m512d zr2 = _mm512_sub_pd(z0_vector, _mm512_load_pd(zs + i + 8));
				result_vec = _mm512_fmadd_pd(zr, zr, result_vec);
				result_vec2 = _mm512_fmadd_pd(zr2, zr2, result_vec2);

				// Compare real distance <= radius
				__mmask8 mask = _mm512_cmp_pd_mask(result_vec, radius_vector, _CMP_LE_OQ);
				__mmask8 mask2 = _mm512_cmp_pd_mask(result_vec2, radius_vector, _CMP_LE_OQ);

				if (mask) {
					// Save matching results
					_mm512_mask_compressstoreu_epi64(indices + elements_count, mask, base_idx);
					_mm512_mask_compressstoreu_pd(distances + elements_count, mask, result_vec);

					// Count valid elements
					elements_count += __builtin_popcount(mask);
				}
				if (mask2) {
					// Save matching results
					_mm512_mask_compressstoreu_epi64(indices + elements_count, mask2,
									 _mm512_add_epi64(base_idx, increment));
					_mm512_mask_compressstoreu_pd(distances + elements_count, mask2, result_vec2);

					// Count valid elements
					elements_count += __builtin_popcount(mask2);
				}
			}
		}
		// Increment indices
		base_idx = _mm512_add_epi64(base_idx, increment_16);
	}
	return elements_count;
}
#endif
