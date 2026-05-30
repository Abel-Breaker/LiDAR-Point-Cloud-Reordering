#include "avx2.h"
#include <immintrin.h>

#if defined(__AVX2__) && defined(USE_FLOAT)
index_t tfg_radius_search_avx2(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
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
	for (index_t i = 0; i + 15 < window; i += 16) {

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
#elif defined(__AVX2__)
index_t tfg_radius_search_avx2(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
				 index_t window, index_t search_start_index, data_t x, data_t y, data_t z,
				 data_t radius, index_t *restrict indices, data_t *restrict distances)
{
	index_t elements_count = 0;
	const index_t num_blocks = window / 8;

	// Arrays locales — solo masks (pequeño) y distancias completas (sin comprimir)
	uint8_t local_masks[num_blocks];
	alignas(64) data_t local_distances[window]; // storeu normal, sin comprimir

	__m512d x0_vector = _mm512_set1_pd(x);
	__m512d y0_vector = _mm512_set1_pd(y);
	__m512d z0_vector = _mm512_set1_pd(z);
	__m512d radius_vector = _mm512_set1_pd(radius);

	// --- Fase 1: compute + storeu normal (sin comprimir) ---
	for (index_t i = 0, b = 0; i + 7 < window; i += 8, b++) {
		__m512d dx = _mm512_sub_pd(_mm512_load_pd(xs + i), x0_vector);
		__m512d dy = _mm512_sub_pd(_mm512_load_pd(ys + i), y0_vector);
		__m512d dz = _mm512_sub_pd(_mm512_load_pd(zs + i), z0_vector);

		__m512d r = _mm512_fmadd_pd(dz, dz, _mm512_fmadd_pd(dy, dy, _mm512_mul_pd(dx, dx)));

		local_masks[b] = _mm512_cmp_pd_mask(r, radius_vector, _CMP_LE_OQ);
		if (!local_masks[b])
			continue;
		_mm512_store_pd(local_distances + i, r);
	}

	// --- Fase 2: recorrer bit a bit las masks y compactar ---
	// Fase 2: bucle fijo de 8, el compilador lo unrollea solo
	for (index_t b = 0; b < num_blocks; b++) {
		uint8_t mask = local_masks[b];
		if (!mask)
			continue;

		index_t base = b * 8;
		for (int bit = 0; bit < 8; bit++) {
			if (mask & (1 << bit)) {
				indices[elements_count] = search_start_index + base + bit;
				distances[elements_count] = local_distances[base + bit];
				elements_count++;
			}
		}
	}

	return elements_count;
}
#endif
