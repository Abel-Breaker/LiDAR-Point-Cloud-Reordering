#include "avx512.h"
#include <immintrin.h>

#if defined(__AVX512F__) && defined(USE_FLOAT)
index_t tfg_radius_search_avx512(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
				 index_t window, index_t search_start_index, data_t x, data_t y, data_t z,
				 data_t radius, index_t *restrict indices, data_t *restrict distances)
{
	index_t elements_count = 0;

	// Create a vector with the position of the point to compare (x0, y0, z0)
	__m512 x0_vector = _mm512_set1_ps(x);
	__m512 y0_vector = _mm512_set1_ps(y);
	__m512 z0_vector = _mm512_set1_ps(z);

	// Create a vector with the radius
	__m512 radius_vector = _mm512_set1_ps(radius);

	// Create base index (where search starts)
	__m512i base_idx = _mm512_set_epi32(
	    search_start_index + 15, search_start_index + 14, search_start_index + 13, search_start_index + 12,
	    search_start_index + 11, search_start_index + 10, search_start_index + 9, search_start_index + 8,
	    search_start_index + 7, search_start_index + 6, search_start_index + 5, search_start_index + 4,
	    search_start_index + 3, search_start_index + 2, search_start_index + 1, search_start_index + 0);

	__m512i increment = _mm512_set1_epi32(16);

	// AVX-512 optimized search
	for (index_t i=0; i + 15 < window; i += 16) {

		// Load data from point cloud
		__m512 x1_vector = _mm512_load_ps(xs + i);
		__m512 y1_vector = _mm512_load_ps(ys + i);
		__m512 z1_vector = _mm512_load_ps(zs + i);

		// Differences
		__m512 x_vector_result = _mm512_sub_ps(x1_vector, x0_vector);
		__m512 y_vector_result = _mm512_sub_ps(y1_vector, y0_vector);
		__m512 z_vector_result = _mm512_sub_ps(z1_vector, z0_vector);

		// Squared Euclidean distance
		__m512 result_vec = _mm512_mul_ps(x_vector_result, x_vector_result);
		result_vec = _mm512_fmadd_ps(y_vector_result, y_vector_result, result_vec);
		result_vec = _mm512_fmadd_ps(z_vector_result, z_vector_result, result_vec);

		// Compare distance <= radius
		__mmask16 mask = _mm512_cmp_ps_mask(result_vec, radius_vector, _CMP_LE_OQ);

		if (mask == 0)
			continue;

		// Save matching results
		_mm512_mask_compressstoreu_epi32(indices + elements_count, mask, base_idx);
		_mm512_mask_compressstoreu_ps(distances + elements_count, mask, result_vec);

		// Count valid elements
		elements_count += __builtin_popcount(mask);

		// Increment indices
		base_idx = _mm512_add_epi32(base_idx, increment);
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
	__m512d x0_vector = _mm512_set1_pd(x);
	__m512d y0_vector = _mm512_set1_pd(y);
	__m512d z0_vector = _mm512_set1_pd(z);

	// Create a vector with the radius
	__m512d radius_vector = _mm512_set1_pd(radius);

	// Create base index (where search starts)
	__m512i base_idx = _mm512_set_epi64(search_start_index + 7, search_start_index + 6, search_start_index + 5,
					    search_start_index + 4, search_start_index + 3, search_start_index + 2,
					    search_start_index + 1, search_start_index + 0);

	__m512i increment = _mm512_set1_epi64(8);

	// AVX-512 optimized search
	for (index_t i=0; i + 7 < window; i += 8) {

		// Load data from point cloud
		__m512d x1_vector = _mm512_load_pd(xs + i);
		__m512d y1_vector = _mm512_load_pd(ys + i);
		__m512d z1_vector = _mm512_load_pd(zs + i);

		// Differences
		__m512d x_vector_result = _mm512_sub_pd(x1_vector, x0_vector);
		__m512d y_vector_result = _mm512_sub_pd(y1_vector, y0_vector);
		__m512d z_vector_result = _mm512_sub_pd(z1_vector, z0_vector);

		// Squared Euclidean distance
		__m512d result_vec = _mm512_mul_pd(x_vector_result, x_vector_result);
		result_vec = _mm512_fmadd_pd(y_vector_result, y_vector_result, result_vec);
		result_vec = _mm512_fmadd_pd(z_vector_result, z_vector_result, result_vec);

		// Compare distance <= radius
		__mmask8 mask = _mm512_cmp_pd_mask(result_vec, radius_vector, _CMP_LE_OQ);

		if (mask == 0)
			continue;

		// Save matching results
		_mm512_mask_compressstoreu_epi64(indices + elements_count, mask, base_idx);
		_mm512_mask_compressstoreu_pd(distances + elements_count, mask, result_vec);

		// Count valid elements
		elements_count += __builtin_popcount(mask);

		// Increment indices
		base_idx = _mm512_add_epi64(base_idx, increment);
	}

	return elements_count;
}
#endif
