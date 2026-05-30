#include "avx2.h"
#include <immintrin.h>

#if defined(__AVX2__) && defined(USE_FLOAT)
index_t tfg_radius_search_avx2(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
			       index_t window, index_t search_start_index, data_t x, data_t y, data_t z, data_t radius,
			       index_t *restrict indices, data_t *restrict distances)
{
	index_t elements_count = 0;

	const __m256 vx = _mm256_set1_ps(x);
    const __m256 vy = _mm256_set1_ps(y);
    const __m256 vz = _mm256_set1_ps(z);
	__m256 radius_vector = _mm256_set1_ps(radius);

	for (index_t i = 0; i + 7 < window; i += 8) {
		// Load + differences
		__m256 dx = _mm256_sub_ps(_mm256_load_ps(xs + i), vx);
		__m256 dy = _mm256_sub_ps(_mm256_load_ps(ys + i), vy);
		__m256 dz = _mm256_sub_ps(_mm256_load_ps(zs + i), vz);

		// Squared Euclidean distance
		__m256 r = _mm256_fmadd_ps(dz, dz, _mm256_fmadd_ps(dy, dy, _mm256_mul_ps(dx, dx)));

		int mask = _mm256_movemask_ps(_mm256_cmp_ps(r, radius_vector, _CMP_LE_OQ));

		if (!mask) continue;

        alignas(32) data_t buf[8];
        _mm256_store_ps(buf, r);

        // Manual compact-store
        while (mask) {
            int bit = __builtin_ctz(mask);
            indices[elements_count]   = search_start_index + i + bit;
            distances[elements_count] = buf[bit];
            elements_count++;
            mask &= mask - 1;
        }
	}

	return elements_count;
}
#elif defined(__AVX2__)
index_t tfg_radius_search_avx2(const data_t *restrict xs, const data_t *restrict ys, const data_t *restrict zs,
			       index_t window, index_t search_start_index, data_t x, data_t y, data_t z, data_t radius,
			       index_t *restrict indices, data_t *restrict distances)
{
	index_t elements_count = 0;
	__m256d radius_vector = _mm256_set1_pd(radius);

	for (index_t i = 0; i + 3 < window; i += 4) {
		// Load + differences
		__m256d dx = _mm256_sub_pd(_mm256_load_pd(xs + i), _mm256_set1_pd(x));
		__m256d dy = _mm256_sub_pd(_mm256_load_pd(ys + i), _mm256_set1_pd(y));
		__m256d dz = _mm256_sub_pd(_mm256_load_pd(zs + i), _mm256_set1_pd(z));

		// Squared Euclidean distance
		__m256d r = _mm256_fmadd_pd(dz, dz, _mm256_fmadd_pd(dy, dy, _mm256_mul_pd(dx, dx)));

		int mask = _mm256_movemask_pd(_mm256_cmp_pd(r, radius_vector, _CMP_LE_OQ));

		if (!mask) continue;

        alignas(32) data_t buf[4];
        _mm256_store_pd(buf, r);

        // Manual compact-store
        while (mask) {
            int bit = __builtin_ctz(mask);
            indices[elements_count]   = search_start_index + i + bit;
            distances[elements_count] = buf[bit];
            elements_count++;
            mask &= mask - 1;
        }
	}

	return elements_count;
}
#endif
